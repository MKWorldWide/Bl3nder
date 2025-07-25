/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup pythonintern
 *
 * This file defines utility methods for `bpy.types.Gizmo`.
 */

#include <Python.h>
#include <cstddef>

#include "MEM_guardedalloc.h"

#include "BLI_alloca.h"
#include "BLI_utildefines.h"

#include "WM_types.hh"

#include "bpy_capi_utils.hh"
#include "bpy_rna_gizmo.hh"

#include "../generic/py_capi_utils.hh"
#include "../generic/python_compat.hh" /* IWYU pragma: keep. */

#include "RNA_access.hh"
#include "RNA_prototypes.hh"

#include "bpy_rna.hh"

/* -------------------------------------------------------------------- */
/** \name Parsing Utility Functions
 *
 * Functions used as callbacks for #PyArg_ParseTuple `O&` format string.
 * \{ */

struct BPyGizmoWithTarget {
  wmGizmo *gz; /* Must be first. */
  wmGizmoProperty *gz_prop;
};

struct BPyGizmoWithTargetType {
  wmGizmo *gz; /* Must be first. */
  const wmGizmoPropertyType *gz_prop_type;
};

static int py_rna_gizmo_parse(PyObject *o, void *p)
{
  /* No type checking (this is `self` not a user defined argument). */
  BLI_assert(BPy_StructRNA_Check(o));
  BLI_assert(RNA_struct_is_a(((const BPy_StructRNA *)o)->ptr->type, &RNA_Gizmo));

  wmGizmo **gz_p = static_cast<wmGizmo **>(p);
  *gz_p = static_cast<wmGizmo *>(((const BPy_StructRNA *)o)->ptr->data);
  return 1;
}

static int py_rna_gizmo_target_id_parse(PyObject *o, void *p)
{
  BPyGizmoWithTarget *gizmo_with_target = static_cast<BPyGizmoWithTarget *>(p);
  /* Must be set by `py_rna_gizmo_parse`. */
  wmGizmo *gz = gizmo_with_target->gz;
  BLI_assert(gz != nullptr);

  if (!PyUnicode_Check(o)) {
    PyErr_Format(PyExc_TypeError, "expected a string (got %.200s)", Py_TYPE(o)->tp_name);
    return 0;
  }
  const char *gz_prop_id = PyUnicode_AsUTF8(o);
  wmGizmoProperty *gz_prop = WM_gizmo_target_property_find(gz, gz_prop_id);
  if (gz_prop == nullptr) {
    PyErr_Format(PyExc_ValueError,
                 "Gizmo target property '%s.%s' not found!",
                 gz->type->idname,
                 gz_prop_id);
    return 0;
  }
  gizmo_with_target->gz_prop = gz_prop;
  return 1;
}

static int py_rna_gizmo_target_id_parse_and_ensure_is_valid(PyObject *o, void *p)
{
  if (py_rna_gizmo_target_id_parse(o, p) == 0) {
    return 0;
  }
  BPyGizmoWithTarget *gizmo_with_target = static_cast<BPyGizmoWithTarget *>(p);
  wmGizmo *gz = gizmo_with_target->gz;
  wmGizmoProperty *gz_prop = gizmo_with_target->gz_prop;
  if (!WM_gizmo_target_property_is_valid(gz_prop)) {
    const char *gz_prop_id = PyUnicode_AsUTF8(o);
    PyErr_Format(PyExc_ValueError,
                 "Gizmo target property '%s.%s' has not been initialized, "
                 "Call \"target_set_prop\" first!",
                 gz->type->idname,
                 gz_prop_id);
    return 0;
  }
  return 1;
}

static int py_rna_gizmo_target_type_id_parse(PyObject *o, void *p)
{
  BPyGizmoWithTargetType *gizmo_with_target = static_cast<BPyGizmoWithTargetType *>(p);
  /* Must be set first. */
  wmGizmo *gz = gizmo_with_target->gz;
  BLI_assert(gz != nullptr);

  if (!PyUnicode_Check(o)) {
    PyErr_Format(PyExc_TypeError, "expected a string (got %.200s)", Py_TYPE(o)->tp_name);
    return 0;
  }
  const char *gz_prop_id = PyUnicode_AsUTF8(o);
  const wmGizmoPropertyType *gz_prop_type = WM_gizmotype_target_property_find(gz->type,
                                                                              gz_prop_id);
  if (gz_prop_type == nullptr) {
    PyErr_Format(PyExc_ValueError,
                 "Gizmo target property '%s.%s' not found!",
                 gz->type->idname,
                 gz_prop_id);
    return 0;
  }
  gizmo_with_target->gz_prop_type = gz_prop_type;
  return 1;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Gizmo Target Property Define API
 * \{ */

enum {
  BPY_GIZMO_FN_SLOT_GET = 0,
  BPY_GIZMO_FN_SLOT_SET,
  BPY_GIZMO_FN_SLOT_RANGE_GET,
};
#define BPY_GIZMO_FN_SLOT_LEN (BPY_GIZMO_FN_SLOT_RANGE_GET + 1)

struct BPyGizmoHandlerUserData {

  PyObject *fn_slots[BPY_GIZMO_FN_SLOT_LEN];
};

static void py_rna_gizmo_handler_get_cb(const wmGizmo * /*gz*/,
                                        wmGizmoProperty *gz_prop,
                                        void *value_p)
{
  const PyGILState_STATE gilstate = PyGILState_Ensure();

  BPyGizmoHandlerUserData *data = static_cast<BPyGizmoHandlerUserData *>(
      gz_prop->custom_func.user_data);
  PyObject *ret = PyObject_CallObject(data->fn_slots[BPY_GIZMO_FN_SLOT_GET], nullptr);
  if (ret == nullptr) {
    goto fail;
  }

  if (gz_prop->type->data_type == PROP_FLOAT) {
    float *value = static_cast<float *>(value_p);
    if (gz_prop->type->array_length == 1) {
      if ((*value = PyFloat_AsDouble(ret)) == -1.0f && PyErr_Occurred()) {
        goto fail;
      }
    }
    else {
      if (PyC_AsArray(value,
                      sizeof(*value),
                      ret,
                      gz_prop->type->array_length,
                      &PyFloat_Type,
                      "Gizmo get callback: ") == -1)
      {
        goto fail;
      }
    }
  }
  else {
    PyErr_SetString(PyExc_AttributeError, "internal error, unsupported type");
    goto fail;
  }

  Py_DECREF(ret);

  PyGILState_Release(gilstate);
  return;

fail:
  PyErr_Print();
  PyErr_Clear();

  Py_XDECREF(ret);

  PyGILState_Release(gilstate);
}

static void py_rna_gizmo_handler_set_cb(const wmGizmo * /*gz*/,
                                        wmGizmoProperty *gz_prop,
                                        const void *value_p)
{
  const PyGILState_STATE gilstate = PyGILState_Ensure();

  BPyGizmoHandlerUserData *data = static_cast<BPyGizmoHandlerUserData *>(
      gz_prop->custom_func.user_data);

  PyObject *args = PyTuple_New(1);
  PyObject *ret;

  if (gz_prop->type->data_type == PROP_FLOAT) {
    const float *value = static_cast<const float *>(value_p);
    PyObject *py_value;
    if (gz_prop->type->array_length == 1) {
      py_value = PyFloat_FromDouble(*value);
    }
    else {
      py_value = PyC_Tuple_PackArray_F32(value, gz_prop->type->array_length);
    }
    if (py_value == nullptr) {
      goto fail;
    }
    PyTuple_SET_ITEM(args, 0, py_value);
  }
  else {
    PyErr_SetString(PyExc_AttributeError, "internal error, unsupported type");
    goto fail;
  }

  ret = PyObject_CallObject(data->fn_slots[BPY_GIZMO_FN_SLOT_SET], args);
  if (ret == nullptr) {
    goto fail;
  }
  Py_DECREF(args);
  Py_DECREF(ret);

  PyGILState_Release(gilstate);
  return;

fail:
  PyErr_Print();
  PyErr_Clear();

  Py_DECREF(args);

  PyGILState_Release(gilstate);
}

static void py_rna_gizmo_handler_range_get_cb(const wmGizmo * /*gz*/,
                                              wmGizmoProperty *gz_prop,
                                              void *value_p)
{
  const PyGILState_STATE gilstate = PyGILState_Ensure();

  BPyGizmoHandlerUserData *data = static_cast<BPyGizmoHandlerUserData *>(
      gz_prop->custom_func.user_data);

  PyObject *ret = PyObject_CallObject(data->fn_slots[BPY_GIZMO_FN_SLOT_RANGE_GET], nullptr);
  if (ret == nullptr) {
    goto fail;
  }

  if (!PyTuple_Check(ret)) {
    PyErr_Format(PyExc_TypeError, "Expected a tuple, not %.200s", Py_TYPE(ret)->tp_name);
    goto fail;
  }

  if (PyTuple_GET_SIZE(ret) != 2) {
    PyErr_Format(PyExc_TypeError, "Expected a tuple of size 2, not %d", PyTuple_GET_SIZE(ret));
    goto fail;
  }

  if (gz_prop->type->data_type == PROP_FLOAT) {
    float range[2];
    for (int i = 0; i < 2; i++) {
      if (((range[i] = PyFloat_AsDouble(PyTuple_GET_ITEM(ret, i))) == -1.0f && PyErr_Occurred()) ==
          0)
      {
        /* pass */
      }
      else {
        goto fail;
      }
    }
    memcpy(value_p, range, sizeof(range));
  }
  else {
    PyErr_SetString(PyExc_AttributeError, "internal error, unsupported type");
    goto fail;
  }

  Py_DECREF(ret);
  PyGILState_Release(gilstate);
  return;

fail:
  PyErr_Print();
  PyErr_Clear();

  Py_XDECREF(ret);

  PyGILState_Release(gilstate);
}

static void py_rna_gizmo_handler_free_cb(const wmGizmo * /*gz*/, wmGizmoProperty *gz_prop)
{
  const PyGILState_STATE gilstate = PyGILState_Ensure();

  BPyGizmoHandlerUserData *data = static_cast<BPyGizmoHandlerUserData *>(
      gz_prop->custom_func.user_data);

  for (int i = 0; i < BPY_GIZMO_FN_SLOT_LEN; i++) {
    Py_XDECREF(data->fn_slots[i]);
  }
  PyGILState_Release(gilstate);

  MEM_freeN(data);
}

PyDoc_STRVAR(
    /* Wrap. */
    bpy_gizmo_target_set_handler_doc,
    ".. method:: target_set_handler(target, get, set, range=None):\n"
    "\n"
    "   Assigns callbacks to a gizmos property.\n"
    "\n"
    "   :arg target: Target property name.\n"
    "   :type target: str\n"
    "   :arg get: Function that returns the value for this property (single value or sequence).\n"
    "   :type get: Callable[[], float | Sequence[float]]\n"
    "   :arg set: Function that takes a single value argument and applies it.\n"
    "   :type set: Callable[[tuple[float, ...]], Any]\n"
    "   :arg range: Function that returns a (min, max) tuple for gizmos that use a range. "
    "The returned value is not used.\n"
    "   :type range: callable\n");
static PyObject *bpy_gizmo_target_set_handler(PyObject * /*self*/, PyObject *args, PyObject *kw)
{
  struct {
    BPyGizmoWithTargetType gz_with_target_type;
    PyObject *py_fn_slots[BPY_GIZMO_FN_SLOT_LEN];
  } params = {
      {nullptr, nullptr},
      {nullptr},
  };

  /* NOTE: this is a counter-part to functions:
   * 'Gizmo.target_set_prop & target_set_operator'
   * (see: rna_wm_gizmo_api.cc). conventions should match. */
  static const char *const _keywords[] = {"self", "target", "get", "set", "range", nullptr};
  static _PyArg_Parser _parser = {
      PY_ARG_PARSER_HEAD_COMPAT()
      "O&" /* `self` */
      "O&" /* `target` */
      "|$" /* Optional keyword only arguments. */
      "O"  /* `get` */
      "O"  /* `set` */
      "O"  /* `range` */
      ":target_set_handler",
      _keywords,
      nullptr,
  };

  BPyGizmoHandlerUserData *data;
  const wmGizmoPropertyType *gz_prop_type;
  wmGizmo *gz;

  if (!_PyArg_ParseTupleAndKeywordsFast(args,
                                        kw,
                                        &_parser,
                                        /* `self` */
                                        py_rna_gizmo_parse,
                                        &params.gz_with_target_type.gz,
                                        /* `target` */
                                        py_rna_gizmo_target_type_id_parse,
                                        &params.gz_with_target_type,
                                        /* `get/set/range` */
                                        &params.py_fn_slots[BPY_GIZMO_FN_SLOT_GET],
                                        &params.py_fn_slots[BPY_GIZMO_FN_SLOT_SET],
                                        &params.py_fn_slots[BPY_GIZMO_FN_SLOT_RANGE_GET]))
  {
    goto fail;
  }

  gz = params.gz_with_target_type.gz;
  gz_prop_type = params.gz_with_target_type.gz_prop_type;

  {
    const int slots_required = 2;
    const int slots_start = 2;
    for (int i = 0; i < BPY_GIZMO_FN_SLOT_LEN; i++) {
      if (params.py_fn_slots[i] == nullptr) {
        if (i < slots_required) {
          PyErr_Format(PyExc_ValueError, "Argument '%s' not given", _keywords[slots_start + i]);
          goto fail;
        }
      }
      else if (!PyCallable_Check(params.py_fn_slots[i])) {
        PyErr_Format(PyExc_ValueError, "Argument '%s' not callable", _keywords[slots_start + i]);
        goto fail;
      }
    }
  }

  data = MEM_callocN<BPyGizmoHandlerUserData>(__func__);

  for (int i = 0; i < BPY_GIZMO_FN_SLOT_LEN; i++) {
    data->fn_slots[i] = params.py_fn_slots[i];
    Py_XINCREF(params.py_fn_slots[i]);
  }

  {
    wmGizmoPropertyFnParams fn_params{};
    fn_params.value_get_fn = py_rna_gizmo_handler_get_cb;
    fn_params.value_set_fn = py_rna_gizmo_handler_set_cb;
    fn_params.range_get_fn = py_rna_gizmo_handler_range_get_cb;
    fn_params.free_fn = py_rna_gizmo_handler_free_cb;
    fn_params.user_data = data;
    WM_gizmo_target_property_def_func_ptr(gz, gz_prop_type, &fn_params);
  }

  Py_RETURN_NONE;

fail:
  return nullptr;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Gizmo Target Property Access API
 * \{ */

PyDoc_STRVAR(
    /* Wrap. */
    bpy_gizmo_target_get_value_doc,
    ".. method:: target_get_value(target):\n"
    "\n"
    "   Get the value of this target property.\n"
    "\n"
    "   :arg target: Target property name.\n"
    "   :type target: str\n"
    "   :return: The value of the target property as a value or array based on the target type.\n"
    "   :rtype: float | tuple[float, ...]\n");
static PyObject *bpy_gizmo_target_get_value(PyObject * /*self*/, PyObject *args, PyObject *kw)
{
  struct {
    BPyGizmoWithTarget gz_with_target;
  } params = {
      {nullptr, nullptr},
  };

  static const char *const _keywords[] = {"self", "target", nullptr};
  static _PyArg_Parser _parser = {
      PY_ARG_PARSER_HEAD_COMPAT()
      "O&" /* `self` */
      "O&" /* `target` */
      ":target_get_value",
      _keywords,
      nullptr,
  };

  int array_len;
  wmGizmo *gz;
  wmGizmoProperty *gz_prop;

  if (!_PyArg_ParseTupleAndKeywordsFast(args,
                                        kw,
                                        &_parser,
                                        /* `self` */
                                        py_rna_gizmo_parse,
                                        &params.gz_with_target.gz,
                                        /* `target` */
                                        py_rna_gizmo_target_id_parse_and_ensure_is_valid,
                                        &params.gz_with_target))
  {
    goto fail;
  }

  gz = params.gz_with_target.gz;
  gz_prop = params.gz_with_target.gz_prop;

  array_len = WM_gizmo_target_property_array_length(gz, gz_prop);
  switch (gz_prop->type->data_type) {
    case PROP_FLOAT: {
      if (array_len != 0) {
        float *value = static_cast<float *>(BLI_array_alloca(value, array_len));
        WM_gizmo_target_property_float_get_array(gz, gz_prop, value);
        return PyC_Tuple_PackArray_F32(value, array_len);
      }

      const float value = WM_gizmo_target_property_float_get(gz, gz_prop);
      return PyFloat_FromDouble(value);
    }
    default: {
      PyErr_SetString(PyExc_RuntimeError, "Not yet supported type");
      goto fail;
    }
  }

fail:
  return nullptr;
}

PyDoc_STRVAR(
    /* Wrap. */
    bpy_gizmo_target_set_value_doc,
    ".. method:: target_set_value(target):\n"
    "\n"
    "   Set the value of this target property.\n"
    "\n"
    "   :arg target: Target property name.\n"
    "   :type target: str\n");
static PyObject *bpy_gizmo_target_set_value(PyObject * /*self*/, PyObject *args, PyObject *kw)
{
  struct {
    BPyGizmoWithTarget gz_with_target;
    PyObject *value;
  } params = {
      {nullptr, nullptr},
      nullptr,
  };

  static const char *const _keywords[] = {"self", "target", "value", nullptr};
  static _PyArg_Parser _parser = {
      PY_ARG_PARSER_HEAD_COMPAT()
      "O&" /* `self` */
      "O&" /* `target` */
      "O"  /* `value` */
      ":target_set_value",
      _keywords,
      nullptr,
  };

  wmGizmo *gz;
  wmGizmoProperty *gz_prop;
  int array_len;

  if (!_PyArg_ParseTupleAndKeywordsFast(args,
                                        kw,
                                        &_parser,
                                        /* `self` */
                                        py_rna_gizmo_parse,
                                        &params.gz_with_target.gz,
                                        /* `target` */
                                        py_rna_gizmo_target_id_parse_and_ensure_is_valid,
                                        &params.gz_with_target,
                                        /* `value` */
                                        &params.value))
  {
    goto fail;
  }

  gz = params.gz_with_target.gz;
  gz_prop = params.gz_with_target.gz_prop;

  array_len = WM_gizmo_target_property_array_length(gz, gz_prop);
  switch (gz_prop->type->data_type) {
    case PROP_FLOAT: {
      if (array_len != 0) {
        float *value = static_cast<float *>(BLI_array_alloca(value, array_len));
        if (PyC_AsArray(value,
                        sizeof(*value),
                        params.value,
                        gz_prop->type->array_length,
                        &PyFloat_Type,
                        "Gizmo target property array: ") == -1)
        {
          goto fail;
        }
        WM_gizmo_target_property_float_set_array(BPY_context_get(), gz, gz_prop, value);
      }
      else {
        float value;
        if ((value = PyFloat_AsDouble(params.value)) == -1.0f && PyErr_Occurred()) {
          goto fail;
        }
        WM_gizmo_target_property_float_set(BPY_context_get(), gz, gz_prop, value);
      }
      Py_RETURN_NONE;
    }
    default: {
      PyErr_SetString(PyExc_RuntimeError, "Not yet supported type");
      goto fail;
    }
  }

fail:
  return nullptr;
}

PyDoc_STRVAR(
    /* Wrap. */
    bpy_gizmo_target_get_range_doc,
    ".. method:: target_get_range(target):\n"
    "\n"
    "   Get the range for this target property.\n"
    "\n"
    "   :arg target: Target property name.\n"
    "   :return: The range of this property (min, max).\n"
    "   :rtype: tuple[float, float]\n");
static PyObject *bpy_gizmo_target_get_range(PyObject * /*self*/, PyObject *args, PyObject *kw)
{
  struct {
    BPyGizmoWithTarget gz_with_target;
  } params = {
      {nullptr, nullptr},
  };

  static const char *const _keywords[] = {"self", "target", nullptr};
  static _PyArg_Parser _parser = {
      PY_ARG_PARSER_HEAD_COMPAT()
      "O&" /* `self` */
      "O&" /* `target` */
      ":target_get_range",
      _keywords,
      nullptr,
  };

  wmGizmo *gz;
  wmGizmoProperty *gz_prop;

  if (!_PyArg_ParseTupleAndKeywordsFast(args,
                                        kw,
                                        &_parser,
                                        /* `self` */
                                        py_rna_gizmo_parse,
                                        &params.gz_with_target.gz,
                                        /* `target` */
                                        py_rna_gizmo_target_id_parse_and_ensure_is_valid,
                                        &params.gz_with_target))
  {
    goto fail;
  }

  gz = params.gz_with_target.gz;
  gz_prop = params.gz_with_target.gz_prop;

  switch (gz_prop->type->data_type) {
    case PROP_FLOAT: {
      float range[2];
      WM_gizmo_target_property_float_range_get(gz, gz_prop, range);
      return PyC_Tuple_PackArray_F32(range, 2);
    }
    default: {
      PyErr_SetString(PyExc_RuntimeError, "Not yet supported type");
      goto fail;
    }
  }

fail:
  return nullptr;
}

/** \} */

/* -------------------------------------------------------------------- */
/** \name Gizmo Module
 * \{ */

bool BPY_rna_gizmo_module(PyObject *mod_par)
{

#ifdef __GNUC__
#  ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wcast-function-type"
#  else
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wcast-function-type"
#  endif
#endif

  static PyMethodDef method_def_array[] = {
      /* Gizmo Target Property Define API */
      {"target_set_handler",
       (PyCFunction)bpy_gizmo_target_set_handler,
       METH_VARARGS | METH_KEYWORDS,
       bpy_gizmo_target_set_handler_doc},
      /* Gizmo Target Property Access API */
      {"target_get_value",
       (PyCFunction)bpy_gizmo_target_get_value,
       METH_VARARGS | METH_KEYWORDS,
       bpy_gizmo_target_get_value_doc},
      {"target_set_value",
       (PyCFunction)bpy_gizmo_target_set_value,
       METH_VARARGS | METH_KEYWORDS,
       bpy_gizmo_target_set_value_doc},
      {"target_get_range",
       (PyCFunction)bpy_gizmo_target_get_range,
       METH_VARARGS | METH_KEYWORDS,
       bpy_gizmo_target_get_range_doc},
      /* no sentinel needed. */
  };

#ifdef __GNUC__
#  ifdef __clang__
#    pragma clang diagnostic pop
#  else
#    pragma GCC diagnostic pop
#  endif
#endif

  for (int i = 0; i < ARRAY_SIZE(method_def_array); i++) {
    PyMethodDef *m = &method_def_array[i];
    PyObject *func = PyCFunction_New(m, nullptr);
    PyObject *func_inst = PyInstanceMethod_New(func);
    char name_prefix[128];
    PyOS_snprintf(name_prefix, sizeof(name_prefix), "_rna_gizmo_%s", m->ml_name);
    /* TODO: return a type that binds nearly to a method. */
    PyModule_AddObject(mod_par, name_prefix, func_inst);
  }

  return false;
}

/** \} */
