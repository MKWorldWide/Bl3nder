/* SPDX-FileCopyrightText: 2013 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup depsgraph
 *
 * Methods for constructing depsgraph.
 */

#include "BLI_listbase.h"
#include "BLI_utildefines.h"

#include "DNA_cachefile_types.h"
#include "DNA_camera_types.h"
#include "DNA_collection_types.h"
#include "DNA_node_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "BKE_collection.hh"
#include "BKE_global.hh"
#include "BKE_main.hh"
#include "BKE_scene.hh"

#include "DEG_depsgraph.hh"
#include "DEG_depsgraph_build.hh"
#include "DEG_depsgraph_debug.hh"

#include "builder/deg_builder_relations.h"
#include "builder/pipeline_all_objects.h"
#include "builder/pipeline_compositor.h"
#include "builder/pipeline_from_collection.h"
#include "builder/pipeline_from_ids.h"
#include "builder/pipeline_render.h"
#include "builder/pipeline_view_layer.h"

#include "intern/debug/deg_debug.h"

#include "intern/node/deg_node.hh"
#include "intern/node/deg_node_component.hh"
#include "intern/node/deg_node_id.hh"
#include "intern/node/deg_node_operation.hh"

#include "intern/depsgraph_registry.hh"
#include "intern/depsgraph_relation.hh"
#include "intern/depsgraph_tag.hh"
#include "intern/depsgraph_type.hh"

/* ****************** */
/* External Build API */

namespace deg = blender::deg;

static deg::NodeType deg_build_scene_component_type(eDepsSceneComponentType component)
{
  switch (component) {
    case DEG_SCENE_COMP_PARAMETERS:
      return deg::NodeType::PARAMETERS;
    case DEG_SCENE_COMP_ANIMATION:
      return deg::NodeType::ANIMATION;
    case DEG_SCENE_COMP_SEQUENCER:
      return deg::NodeType::SEQUENCER;
  }
  return deg::NodeType::UNDEFINED;
}

static deg::DepsNodeHandle *get_node_handle(DepsNodeHandle *node_handle)
{
  return reinterpret_cast<deg::DepsNodeHandle *>(node_handle);
}

void DEG_add_scene_relation(DepsNodeHandle *node_handle,
                            Scene *scene,
                            eDepsSceneComponentType component,
                            const char *description)
{
  deg::NodeType type = deg_build_scene_component_type(component);
  deg::ComponentKey comp_key(&scene->id, type);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(comp_key, deg_node_handle, description);
}

static void add_camera_parameters_relation(DepsNodeHandle *node_handle,
                                           Camera *camera,
                                           const char *description)
{
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg::ComponentKey parameters_key(&camera->id, deg::NodeType::PARAMETERS);
  deg_node_handle->builder->add_node_handle_relation(parameters_key, deg_node_handle, description);
}

void DEG_add_scene_camera_relation(DepsNodeHandle *node_handle,
                                   Scene *scene,
                                   eDepsObjectComponentType component,
                                   const char *description)
{
  if (scene->camera != nullptr) {
    DEG_add_object_relation(node_handle, scene->camera, component, description);
    if (scene->camera->type == OB_CAMERA) {
      add_camera_parameters_relation(
          node_handle, reinterpret_cast<Camera *>(scene->camera->data), description);
    }
  }

  /* Like DepsgraphNodeBuilder::build_scene_camera(), we also need to account for other cameras
   * referenced by markers. */
  LISTBASE_FOREACH (TimeMarker *, marker, &scene->markers) {
    if (!ELEM(marker->camera, nullptr, scene->camera)) {
      DEG_add_object_relation(node_handle, marker->camera, component, description);
      if (marker->camera->type == OB_CAMERA) {
        add_camera_parameters_relation(
            node_handle, reinterpret_cast<Camera *>(marker->camera->data), description);
      }
    }
  }
}

void DEG_add_object_relation(DepsNodeHandle *node_handle,
                             Object *object,
                             eDepsObjectComponentType component,
                             const char *description)
{
  deg::NodeType type = deg::nodeTypeFromObjectComponent(component);
  deg::ComponentKey comp_key(&object->id, type);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(comp_key, deg_node_handle, description);
}

bool DEG_object_has_geometry_component(Object *object)
{
  return deg::geometry_tag_to_component(&object->id) == deg::NodeType::GEOMETRY;
}

void DEG_add_collection_geometry_relation(DepsNodeHandle *node_handle,
                                          Collection *collection,
                                          const char *description)
{
  deg::OperationKey operation_key{
      &collection->id, deg::NodeType::GEOMETRY, deg::OperationCode::GEOMETRY_EVAL_DONE};
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(operation_key, deg_node_handle, description);
}

void DEG_add_collection_geometry_customdata_mask(DepsNodeHandle *node_handle,
                                                 Collection *collection,
                                                 const CustomData_MeshMasks *masks)
{
  FOREACH_COLLECTION_OBJECT_RECURSIVE_BEGIN (collection, ob) {
    DEG_add_customdata_mask(node_handle, ob, masks);
    if (ob->type == OB_EMPTY && ob->instance_collection != nullptr) {
      DEG_add_collection_geometry_customdata_mask(node_handle, ob->instance_collection, masks);
    }
  }
  FOREACH_COLLECTION_OBJECT_RECURSIVE_END;
}

void DEG_add_node_tree_output_relation(DepsNodeHandle *node_handle,
                                       bNodeTree *node_tree,
                                       const char *description)
{
  deg::OperationKey ntree_output_key(
      &node_tree->id, deg::NodeType::NTREE_OUTPUT, deg::OperationCode::NTREE_OUTPUT);
  deg::OperationKey ntree_preprocess_key(&node_tree->id,
                                         deg::NodeType::NTREE_GEOMETRY_PREPROCESS,
                                         deg::OperationCode::NTREE_GEOMETRY_PREPROCESS);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(
      ntree_output_key, deg_node_handle, description);
  deg_node_handle->builder->add_node_handle_relation(
      ntree_preprocess_key, deg_node_handle, description, deg::RELATION_FLAG_NO_FLUSH);
}

void DEG_add_object_cache_relation(DepsNodeHandle *node_handle,
                                   CacheFile *cache_file,
                                   eDepsObjectComponentType component,
                                   const char *description)
{
  deg::NodeType type = deg::nodeTypeFromObjectComponent(component);
  deg::ComponentKey comp_key(&cache_file->id, type);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(comp_key, deg_node_handle, description);
}

void DEG_add_bone_relation(DepsNodeHandle *node_handle,
                           Object *object,
                           const char *bone_name,
                           eDepsObjectComponentType component,
                           const char *description)
{
  deg::NodeType type = deg::nodeTypeFromObjectComponent(component);
  deg::ComponentKey comp_key(&object->id, type, bone_name);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(comp_key, deg_node_handle, description);
}

void DEG_add_object_pointcache_relation(DepsNodeHandle *node_handle,
                                        Object *object,
                                        eDepsObjectComponentType component,
                                        const char *description)
{
  deg::NodeType type = deg::nodeTypeFromObjectComponent(component);
  deg::ComponentKey comp_key(&object->id, type);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg::DepsgraphRelationBuilder *relation_builder = deg_node_handle->builder;
  /* Add relation from source to the node handle. */
  relation_builder->add_node_handle_relation(comp_key, deg_node_handle, description);
  /* Node deduct point cache component and connect source to it. */
  ID *id = DEG_get_id_from_handle(node_handle);
  deg::ComponentKey point_cache_key(id, deg::NodeType::POINT_CACHE);
  deg::Relation *rel = relation_builder->add_relation(comp_key, point_cache_key, "Point Cache");
  if (rel != nullptr) {
    rel->flag |= deg::RELATION_FLAG_FLUSH_USER_EDIT_ONLY;
  }
  else {
    fprintf(stderr, "Error in point cache relation from %s to ^%s.\n", object->id.name, id->name);
  }
}

void DEG_add_generic_id_relation(DepsNodeHandle *node_handle, ID *id, const char *description)
{
  deg::OperationKey operation_key(
      id, deg::NodeType::GENERIC_DATABLOCK, deg::OperationCode::GENERIC_DATABLOCK_UPDATE);
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_node_handle_relation(operation_key, deg_node_handle, description);
}

void DEG_add_depends_on_transform_relation(DepsNodeHandle *node_handle, const char *description)
{
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_depends_on_transform_relation(deg_node_handle, description);
}

void DEG_add_special_eval_flag(DepsNodeHandle *node_handle, ID *id, uint32_t flag)
{
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_special_eval_flag(id, flag);
}

void DEG_add_customdata_mask(DepsNodeHandle *node_handle,
                             Object *object,
                             const CustomData_MeshMasks *masks)
{
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg_node_handle->builder->add_customdata_mask(object, deg::DEGCustomDataMeshMasks(masks));
}

ID *DEG_get_id_from_handle(DepsNodeHandle *node_handle)
{
  deg::DepsNodeHandle *deg_handle = get_node_handle(node_handle);
  return deg_handle->node->owner->owner->id_orig;
}

Depsgraph *DEG_get_graph_from_handle(DepsNodeHandle *node_handle)
{
  deg::DepsNodeHandle *deg_node_handle = get_node_handle(node_handle);
  deg::DepsgraphRelationBuilder *relation_builder = deg_node_handle->builder;
  return reinterpret_cast<Depsgraph *>(relation_builder->getGraph());
}

/* ******************** */
/* Graph Building API's */

void DEG_graph_build_from_view_layer(Depsgraph *graph)
{
  deg::ViewLayerBuilderPipeline builder(graph);
  builder.build();
}

void DEG_graph_build_for_all_objects(Depsgraph *graph)
{
  deg::AllObjectsBuilderPipeline builder(graph);
  builder.build();
}

void DEG_graph_build_for_render_pipeline(Depsgraph *graph)
{
  deg::RenderBuilderPipeline builder(graph);
  builder.build();
}

void DEG_graph_build_for_compositor_preview(Depsgraph *graph, bNodeTree *nodetree)
{
  deg::CompositorBuilderPipeline builder(graph, nodetree);
  builder.build();
}

void DEG_graph_build_from_ids(Depsgraph *graph, blender::Span<ID *> ids)
{
  deg::FromIDsBuilderPipeline builder(graph, ids);
  builder.build();
}

void DEG_graph_build_from_collection(Depsgraph *graph, Collection *collection)
{
  deg::FromCollectionBuilderPipeline builder(graph, collection);
  builder.build();
}

void DEG_graph_tag_relations_update(Depsgraph *graph)
{
  DEG_DEBUG_PRINTF(graph, TAG, "%s: Tagging relations for update.\n", __func__);
  deg::Depsgraph *deg_graph = reinterpret_cast<deg::Depsgraph *>(graph);
  deg_graph->need_update_relations = true;

  /* NOTE: When relations are updated, it's quite possible that we've got new bases in the scene.
   * This means, we need to re-create flat array of bases in view layer. */
  /* TODO(sergey): It is expected that bases manipulation tags scene for update to tag bases array
   * for re-creation. Once it is ensured to happen from all places this implicit tag can be
   * removed. */
  deg::IDNode *id_node = deg_graph->find_id_node(&deg_graph->scene->id);
  if (id_node != nullptr) {
    graph_id_tag_update(deg_graph->bmain,
                        deg_graph,
                        &deg_graph->scene->id,
                        ID_RECALC_BASE_FLAGS | ID_RECALC_HIERARCHY,
                        deg::DEG_UPDATE_SOURCE_RELATIONS);
  }
}

void DEG_graph_relations_update(Depsgraph *graph)
{
  deg::Depsgraph *deg_graph = (deg::Depsgraph *)graph;
  if (!deg_graph->need_update_relations) {
    /* Graph is up to date, nothing to do. */
    return;
  }
  DEG_graph_build_from_view_layer(graph);
}

void DEG_relations_tag_update(Main *bmain)
{
  DEG_GLOBAL_DEBUG_PRINTF(TAG, "%s: Tagging relations for update.\n", __func__);
  for (deg::Depsgraph *depsgraph : deg::get_all_registered_graphs(bmain)) {
    DEG_graph_tag_relations_update(reinterpret_cast<Depsgraph *>(depsgraph));
  }
}
