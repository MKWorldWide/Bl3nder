# SPDX-FileCopyrightText: 2024 Blender Authors
#
# SPDX-License-Identifier: GPL-2.0-or-later

set(INC
  .
  ../../blenkernel
  ../../blenlib
  ../../depsgraph
  ../../draw
  ../../gpu
  ../../imbuf
  ../../makesdna
  ../../makesrna
  ../../nodes
  ../../python
  ../../windowmanager
  ../../../intern/atomic
  ../../../intern/clog
  ../../../intern/guardedalloc
  ../../../extern/fmtlib
)

set(SRC
  lilybear_engine.cc
  lilybear_instance.cc
  lilybear_pipeline.cc
  lilybear_shader.cc
  lilybear_utils.cc
  lilybear_ai_integration.cc
  lilybear_live_rendering.cc
)

set(LIB
  PRIVATE bf::blenfont
  PRIVATE bf::blenkernel
  PRIVATE bf::blenlib
  PRIVATE bf::blenloader
  PRIVATE bf::blentranslation
  PRIVATE bf::bmesh
  PRIVATE bf::depsgraph
  PRIVATE bf::dna
  PRIVATE bf::functions
  PRIVATE bf::gpu
  PRIVATE bf::imbuf
  PRIVATE bf::intern::clog
  PRIVATE bf::intern::guardedalloc
  PRIVATE bf::nodes
  PRIVATE bf::render
  PRIVATE bf::windowmanager
  PRIVATE bf::intern::atomic
  PRIVATE bf::extern::fmtlib
)

# Lilybear-specific libraries (to be added when available)
if(WITH_LILYBEAR)
  list(APPEND LIB
    PRIVATE lilybear_core
    PRIVATE lilybear_ai
    PRIVATE lilybear_rendering
  )
endif()

blender_add_lib(bf_render_lilybear "${SRC}" "${INC}" "${LIB}")

if(WITH_PYTHON)
  set(PYTHON_SRC
    __init__.py
    engine.py
    ai_integration.py
    live_rendering.py
    utils.py
  )

  blender_add_py_lib(bf_render_lilybear_python "${PYTHON_SRC}" "${INC}" "${LIB}")
endif() 