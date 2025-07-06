/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_math_vector.hh"
#include "BLI_rect.h"
#include "BLI_string.h"
#include "BLI_time.h"
#include "BLI_utildefines.h"

#include "GPU_framebuffer.hh"
#include "GPU_texture.hh"
#include "GPU_shader.hh"
#include "GPU_uniform_buffer.hh"

#include "DRW_render.hh"
#include "RE_pipeline.h"

#include "lilybear_types.hh"

namespace blender::lilybear {

/**
 * 🚀 Lilybear Render Instance
 * 
 * Manages a single render instance with:
 * - Live rendering capabilities
 * - AI integration and analysis
 * - Performance monitoring
 * - Progressive rendering pipeline
 */

class Instance {
 private:
  /* Core State */
  bool is_initialized_;
  bool is_rendering_;
  bool live_rendering_active_;
  
  /* Render Context */
  RenderEngine *engine_;
  Depsgraph *depsgraph_;
  Object *camera_object_;
  RenderLayer *render_layer_;
  
  /* Resolution and View */
  int2 resolution_;
  rcti render_rect_;
  rcti visible_rect_;
  
  /* GPU Resources */
  GPUTexture *color_texture_;
  GPUTexture *depth_texture_;
  GPUTexture *normal_texture_;
  GPUTexture *albedo_texture_;
  GPUTexture *roughness_texture_;
  GPUTexture *metallic_texture_;
  
  GPUFramebuffer *main_framebuffer_;
  GPUFramebuffer *accumulation_framebuffer_;
  
  /* Shaders */
  GPUShader *progressive_shader_;
  GPUShader *denoising_shader_;
  GPUShader *compositing_shader_;
  
  /* Uniform Buffers */
  GPUUniformBuf *settings_ubo_;
  GPUUniformBuf *performance_ubo_;
  
  /* Live Rendering */
  LiveRenderingInstance *live_rendering_;
  
  /* AI Integration */
  AIIntegrationInstance *ai_integration_;
  
  /* Performance */
  PerformanceMetrics current_metrics_;
  PerformanceTargets performance_targets_;
  float quality_factor_;
  
  /* Statistics */
  RenderStats render_stats_;
  
 public:
  Instance();
  ~Instance();
  
  /**
   * Initialize render instance
   * Sets up GPU resources and rendering pipeline
   */
  bool init(const int2 &resolution,
            const rcti *rect,
            const rcti *visible_rect,
            RenderEngine *engine,
            Depsgraph *depsgraph,
            Object *camera_object,
            RenderLayer *render_layer);
  
  /**
   * Cleanup resources
   * Frees GPU resources and cleans up state
   */
  void cleanup();
  
  /**
   * Start live rendering
   * Enables real-time progressive rendering
   */
  void start_live_rendering();
  
  /**
   * Stop live rendering
   * Disables real-time rendering and cleans up
   */
  void stop_live_rendering();
  
  /**
   * Render frame with progressive refinement
   * Renders a single frame with live updates
   */
  void render_frame_progressive(RenderEngine *engine,
                               RenderLayer *layer,
                               const char *viewname);
  
  /**
   * Store metadata for AI analysis
   * Saves render metadata for AI processing
   */
  void store_metadata(RenderResult *render_result);
  
  /**
   * Analyze scene for AI suggestions
   * Performs scene analysis and generates AI recommendations
   */
  void analyze_scene_for_ai();
  
  /**
   * Process AI suggestions
   * Applies AI-generated suggestions to the scene
   */
  void process_ai_suggestions(Depsgraph *depsgraph);
  
  /**
   * Apply AI optimizations
   * Applies AI-driven optimizations to render settings
   */
  void apply_ai_optimizations(Scene *scene);
  
  /**
   * Update live rendering quality
   * Adjusts rendering quality based on performance
   */
  void update_live_rendering_quality(float quality_factor);
  
  /**
   * Get performance metrics
   * Returns current performance data
   */
  PerformanceMetrics get_performance_metrics() const;
  
  /**
   * Set performance targets
   * Configures performance targets for adaptive rendering
   */
  void set_performance_targets(const PerformanceTargets &targets);
  
  /**
   * Viewport update
   * Updates viewport for live rendering
   */
  void view_update(bContext *context, Depsgraph *depsgraph);
  
  /**
   * Viewport draw
   * Draws viewport with live rendering
   */
  void view_draw(bContext *context, Depsgraph *depsgraph);
  
  /**
   * Update render passes
   * Updates render passes for the engine
   */
  static void update_passes(RenderEngine *engine, Scene *scene, ViewLayer *view_layer);
  
 private:
  /**
   * Create GPU textures
   * Creates all required GPU textures
   */
  bool create_textures();
  
  /**
   * Create framebuffers
   * Creates all required framebuffers
   */
  bool create_framebuffers();
  
  /**
   * Create shaders
   * Creates all required shaders
   */
  bool create_shaders();
  
  /**
   * Create uniform buffers
   * Creates all required uniform buffers
   */
  bool create_uniform_buffers();
  
  /**
   * Update performance metrics
   * Updates current performance data
   */
  void update_performance_metrics();
  
  /**
   * Apply adaptive quality
   * Adjusts quality based on performance targets
   */
  void apply_adaptive_quality();
  
  /**
   * Render progressive pass
   * Renders a single progressive pass
   */
  void render_progressive_pass(int pass_number);
  
  /**
   * Apply denoising
   * Applies AI denoising to the current frame
   */
  void apply_denoising();
  
  /**
   * Composite final result
   * Composites the final render result
   */
  void composite_final_result();
};

}  // namespace blender::lilybear 