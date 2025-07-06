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

#include "lilybear_types.hh"

namespace blender::lilybear {

/**
 * 🚀 Lilybear Live Rendering System
 * 
 * Provides real-time progressive rendering with:
 * - Adaptive quality adjustment
 * - AI-powered optimization
 * - Real-time feedback and suggestions
 * - Performance monitoring and adaptation
 */

namespace live_rendering {

/**
 * Initialize live rendering system
 * Sets up GPU resources and shaders
 */
void init();

/**
 * Cleanup live rendering system
 * Frees GPU resources and cleans up
 */
void cleanup();

/**
 * Start live rendering mode
 * Enables real-time progressive rendering with AI feedback
 */
void start_live_rendering(RenderEngine *engine);

/**
 * Stop live rendering mode
 * Disables real-time rendering and cleans up resources
 */
void stop_live_rendering(RenderEngine *engine);

/**
 * Update live rendering quality
 * Adjusts rendering quality based on performance and user preferences
 */
void update_live_rendering_quality(RenderEngine *engine, float quality_factor);

}  // namespace live_rendering

/**
 * Live Rendering Instance
 * Manages real-time rendering with progressive refinement
 */
class LiveRenderingInstance {
 private:
  /* Core State */
  bool is_active_;
  bool is_initialized_;
  RenderMode current_mode_;
  
  /* Timing */
  double start_time_;
  double last_update_time_;
  double frame_timer_;
  int frame_count_;
  
  /* Performance */
  PerformanceMetrics current_metrics_;
  PerformanceMetrics target_metrics_;
  float quality_factor_;
  float adaptive_quality_factor_;
  
  /* Rendering */
  int2 resolution_;
  int current_pass_;
  int total_passes_;
  float convergence_ratio_;
  
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
  
  /* AI Integration */
  AIIntegrationInstance *ai_integration_;
  std::vector<AISuggestion> ai_suggestions_;
  
  /* Statistics */
  RenderStats render_stats_;
  
 public:
  LiveRenderingInstance();
  ~LiveRenderingInstance();
  
  /**
   * Initialize live rendering system
   * Sets up GPU resources and shaders
   */
  bool init(const int2 &resolution, const RenderSettings &settings);
  
  /**
   * Start live rendering
   * Begins real-time progressive rendering
   */
  void start();
  
  /**
   * Stop live rendering
   * Stops real-time rendering and cleans up
   */
  void stop();
  
  /**
   * Update live rendering
   * Updates live rendering for viewport
   */
  void update(bContext *context, Depsgraph *depsgraph);
  
  /**
   * Draw live rendering
   * Draws live rendering to viewport
   */
  void draw(bContext *context, Depsgraph *depsgraph);
  
  /**
   * Update quality
   * Updates rendering quality factor
   */
  void update_quality(float quality_factor);
  
  /**
   * Get performance metrics
   * Returns current performance data
   */
  PerformanceMetrics get_performance_metrics() const;
  
  /**
   * Set performance targets
   * Sets performance targets for adaptive rendering
   */
  void set_performance_targets(const PerformanceTargets &targets);
  
  /**
   * Get AI suggestions
   * Returns current AI suggestions
   */
  const std::vector<AISuggestion> &get_ai_suggestions() const;
  
  /**
   * Get render statistics
   * Returns render statistics
   */
  RenderStats get_render_stats() const;
  
  /**
   * Is active
   * Returns whether live rendering is active
   */
  bool is_active() const;
  
 private:
  /**
   * Create textures
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
  
  /**
   * Update AI integration
   * Updates AI integration with current scene data
   */
  void update_ai_integration();
  
  /**
   * Cleanup resources
   * Frees GPU resources and cleans up state
   */
  void cleanup();
};

}  // namespace blender::lilybear 