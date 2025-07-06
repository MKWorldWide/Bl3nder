/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "BLI_rect.h"
#include "BLI_string.h"
#include "BLI_utildefines.h"

#include "GPU_framebuffer.hh"
#include "GPU_texture.hh"

#include "ED_screen.hh"
#include "ED_view3d.hh"

#include "DRW_render.hh"
#include "RE_pipeline.h"

#include "lilybear_engine.h"
#include "lilybear_instance.hh"
#include "lilybear_ai_integration.hh"
#include "lilybear_live_rendering.hh"

namespace blender::lilybear {

/**
 * 🚀 Lilybear Render Engine
 * 
 * Advanced render engine with AI-powered live rendering capabilities.
 * Features:
 * - Real-time progressive rendering
 * - AI-driven scene optimization
 * - Intelligent quality adaptation
 * - Live feedback and suggestions
 */

DrawEngine *Engine::create_instance()
{
  return new Instance();
}

void Engine::free_static()
{
  /* Clean up static resources */
  // TODO: Free shader module when implemented
}

}  // namespace blender::lilybear

using namespace blender::lilybear;

/* -------------------------------------------------------------------- */
/** \name Render Engine Interface
 * \{ */

/**
 * Main render function for Lilybear engine
 * Handles both viewport and final rendering with live capabilities
 */
static void lilybear_render(RenderEngine *engine, Depsgraph *depsgraph)
{
  Instance *instance = nullptr;

  /* Create render pipeline with live rendering support */
  auto lilybear_render_to_image = [&](RenderEngine *engine, RenderLayer *layer, const rcti /*rect*/) {
    Render *render = engine->re;
    Object *camera_original_ob = RE_GetCamera(engine->re);
    const char *viewname = RE_GetActiveRenderView(engine->re);
    int size[2] = {engine->resolution_x, engine->resolution_y};

    /* Create Lilybear instance with live rendering capabilities */
    instance = new Instance();

    rctf view_rect;
    rcti rect;
    RE_GetViewPlane(render, &view_rect, &rect);
    rcti visible_rect = rect;

    /* Initialize with live rendering enabled */
    instance->init(size, &rect, &visible_rect, engine, depsgraph, camera_original_ob, layer);
    
    /* Start live rendering pipeline */
    instance->start_live_rendering();
    
    /* Render frame with progressive refinement */
    instance->render_frame_progressive(engine, layer, viewname);
  };

  /* Store metadata for AI analysis */
  auto lilybear_store_metadata = [&](RenderResult *render_result) {
    if (instance) {
      instance->store_metadata(render_result);
      instance->analyze_scene_for_ai();
    }
  };

  /* Execute render pipeline */
  DRW_render_to_image(engine, depsgraph, lilybear_render_to_image, lilybear_store_metadata);

  /* Cleanup */
  if (instance) {
    instance->stop_live_rendering();
    delete instance;
  }
}

/**
 * Update render passes for Lilybear engine
 * Includes AI-enhanced passes and live rendering buffers
 */
static void lilybear_render_update_passes(RenderEngine *engine, Scene *scene, ViewLayer *view_layer)
{
  Instance::update_passes(engine, scene, view_layer);
}

/**
 * Viewport update for live rendering
 * Provides real-time feedback and AI suggestions
 */
static void lilybear_view_update(RenderEngine *engine, bContext *context, Depsgraph *depsgraph)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->view_update(context, depsgraph);
  }
}

/**
 * Viewport drawing for live rendering
 * Renders progressive updates in real-time
 */
static void lilybear_view_draw(RenderEngine *engine, bContext *context, Depsgraph *depsgraph)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->view_draw(context, depsgraph);
  }
}

/* -------------------------------------------------------------------- */
/** \name Render Engine Type Definition
 * \{ */

/**
 * Lilybear Render Engine Type
 * 
 * Advanced render engine with:
 * - Live rendering capabilities
 * - AI-powered scene optimization
 * - Progressive quality refinement
 * - Real-time feedback and suggestions
 */
RenderEngineType DRW_engine_viewport_lilybear_type = {
    /*next*/ nullptr,
    /*prev*/ nullptr,
    /*idname*/ "LILYBEAR",
    /*name*/ N_("Lilybear"),
    /*flag*/ RE_INTERNAL | RE_USE_PREVIEW | RE_USE_STEREO_VIEWPORT | RE_USE_GPU_CONTEXT | RE_USE_LIVE_RENDERING,
    /*update*/ nullptr,
    /*render*/ &lilybear_render,
    /*render_frame_finish*/ nullptr,
    /*draw*/ nullptr,
    /*bake*/ nullptr,
    /*view_update*/ &lilybear_view_update,
    /*view_draw*/ &lilybear_view_draw,
    /*update_script_node*/ nullptr,
    /*update_render_passes*/ &lilybear_render_update_passes,
    /*update_custom_camera*/ nullptr,
    /*draw_engine*/ nullptr,
    /*rna_ext*/
    {
        /*data*/ nullptr,
        /*srna*/ nullptr,
        /*call*/ nullptr,
    },
};

/* -------------------------------------------------------------------- */
/** \name Engine Registration
 * \{ */

/**
 * Register Lilybear render engine
 * Called during Blender initialization
 */
void register_lilybear_engine()
{
  DRW_engines_register(&DRW_engine_viewport_lilybear_type);
  
  /* Initialize AI integration */
  // TODO: Initialize AI integration when namespace is implemented
  
  /* Initialize live rendering system */
  // TODO: Initialize live rendering when namespace is implemented
  
  CLOG_INFO(LOG_LILYBEAR, 1, "Lilybear render engine registered successfully");
}

/**
 * Unregister Lilybear render engine
 * Called during Blender shutdown
 */
void unregister_lilybear_engine()
{
  /* Cleanup AI integration */
  // TODO: Cleanup AI integration when namespace is implemented
  
  /* Cleanup live rendering system */
  // TODO: Cleanup live rendering when namespace is implemented
  
  CLOG_INFO(LOG_LILYBEAR, 1, "Lilybear render engine unregistered");
}

/* -------------------------------------------------------------------- */
/** \name Utility Functions
 * \{ */

/**
 * Check if Lilybear engine is available
 * Returns true if all required components are loaded
 */
bool is_lilybear_available()
{
  return true; /* TODO: Add proper availability check */
}

/**
 * Get Lilybear engine capabilities
 * Returns bitfield of supported features
 */
uint32_t get_lilybear_capabilities()
{
  uint32_t capabilities = 0;
  
  capabilities |= LILYBEAR_CAP_LIVE_RENDERING;
  capabilities |= LILYBEAR_CAP_AI_INTEGRATION;
  capabilities |= LILYBEAR_CAP_PROGRESSIVE_RENDERING;
  capabilities |= LILYBEAR_CAP_ADAPTIVE_QUALITY;
  capabilities |= LILYBEAR_CAP_REAL_TIME_FEEDBACK;
  
  return capabilities;
}

/**
 * Initialize Lilybear engine settings
 * Sets up default configuration for optimal performance
 */
void init_lilybear_settings()
{
  /* TODO: Initialize default settings */
  CLOG_INFO(LOG_LILYBEAR, 1, "Lilybear settings initialized");
}

/* -------------------------------------------------------------------- */
/** \name AI Integration Interface
 * \{ */

/**
 * Process AI suggestions for scene optimization
 * Analyzes current scene and provides intelligent recommendations
 */
void process_ai_suggestions(RenderEngine *engine, Depsgraph *depsgraph)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->process_ai_suggestions(depsgraph);
  }
}

/**
 * Apply AI optimizations to render settings
 * Automatically adjusts settings for optimal quality/performance balance
 */
void apply_ai_optimizations(RenderEngine *engine, Scene *scene)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->apply_ai_optimizations(scene);
  }
}

/* -------------------------------------------------------------------- */
/** \name Live Rendering Interface
 * \{ */

/**
 * Start live rendering mode
 * Enables real-time progressive rendering with AI feedback
 */
void start_live_rendering(RenderEngine *engine)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->start_live_rendering();
  }
}

/**
 * Stop live rendering mode
 * Disables real-time rendering and cleans up resources
 */
void stop_live_rendering(RenderEngine *engine)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->stop_live_rendering();
  }
}

/**
 * Update live rendering quality
 * Adjusts rendering quality based on performance and user preferences
 */
void update_live_rendering_quality(RenderEngine *engine, float quality_factor)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->update_live_rendering_quality(quality_factor);
  }
}

/* -------------------------------------------------------------------- */
/** \name Performance Monitoring
 * \{ */

/**
 * Get current rendering performance metrics
 * Returns performance data for monitoring and optimization
 */
LilybearPerformanceMetrics get_performance_metrics(RenderEngine *engine)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    return instance->get_performance_metrics();
  }
  
  /* Return default metrics if no instance */
  LilybearPerformanceMetrics metrics = {};
  return metrics;
}

/**
 * Set performance targets for adaptive rendering
 * Configures quality/performance balance based on user preferences
 */
void set_performance_targets(RenderEngine *engine, const LilybearPerformanceTargets &targets)
{
  Instance *instance = static_cast<Instance *>(engine->instance_data);
  if (instance) {
    instance->set_performance_targets(targets);
  }
} 