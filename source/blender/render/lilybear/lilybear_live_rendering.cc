/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

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

#include "lilybear_live_rendering.hh"
#include "lilybear_types.hh"
#include "lilybear_utils.hh"

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

/* -------------------------------------------------------------------- */
/** \name Live Rendering Implementation
 * \{ */

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
  AIIntegration *ai_integration_;
  std::vector<AISuggestion> ai_suggestions_;
  
  /* Statistics */
  RenderStats render_stats_;
  
 public:
  LiveRenderingInstance() : is_active_(false), is_initialized_(false)
  {
    /* Initialize default values */
    quality_factor_ = DEFAULT_QUALITY_FACTOR;
    adaptive_quality_factor_ = DEFAULT_QUALITY_FACTOR;
    current_pass_ = 0;
    total_passes_ = 1;
    convergence_ratio_ = 0.0f;
    frame_count_ = 0;
    
    /* Initialize GPU resources to nullptr */
    color_texture_ = nullptr;
    depth_texture_ = nullptr;
    normal_texture_ = nullptr;
    albedo_texture_ = nullptr;
    roughness_texture_ = nullptr;
    metallic_texture_ = nullptr;
    main_framebuffer_ = nullptr;
    accumulation_framebuffer_ = nullptr;
    progressive_shader_ = nullptr;
    denoising_shader_ = nullptr;
    compositing_shader_ = nullptr;
    settings_ubo_ = nullptr;
    performance_ubo_ = nullptr;
    ai_integration_ = nullptr;
  }
  
  ~LiveRenderingInstance()
  {
    cleanup();
  }
  
  /**
   * Initialize live rendering system
   * Sets up GPU resources and shaders
   */
  bool init(const int2 &resolution, const RenderSettings &settings)
  {
    if (is_initialized_) {
      return true;
    }
    
    resolution_ = resolution;
    
    /* Create GPU textures */
    if (!create_textures()) {
      CLOG_ERROR(LOG_LILYBEAR, "Failed to create textures for live rendering");
      return false;
    }
    
    /* Create framebuffers */
    if (!create_framebuffers()) {
      CLOG_ERROR(LOG_LILYBEAR, "Failed to create framebuffers for live rendering");
      return false;
    }
    
    /* Create shaders */
    if (!create_shaders()) {
      CLOG_ERROR(LOG_LILYBEAR, "Failed to create shaders for live rendering");
      return false;
    }
    
    /* Create uniform buffers */
    if (!create_uniform_buffers()) {
      CLOG_ERROR(LOG_LILYBEAR, "Failed to create uniform buffers for live rendering");
      return false;
    }
    
    /* Initialize AI integration */
    ai_integration_ = new AIIntegration();
    if (!ai_integration_->init()) {
      CLOG_WARN(LOG_LILYBEAR, "Failed to initialize AI integration");
    }
    
    /* Initialize timing */
    start_time_ = BLI_time_now_seconds();
    last_update_time_ = start_time_;
    frame_timer_ = 0.0;
    
    /* Initialize performance targets */
    target_metrics_.fps = settings.target_fps;
    target_metrics_.frame_time = 1.0f / settings.target_fps;
    target_metrics_.gpu_utilization = MAX_GPU_UTILIZATION;
    target_metrics_.memory_utilization = MAX_MEMORY_UTILIZATION;
    
    is_initialized_ = true;
    CLOG_INFO(LOG_LILYBEAR, 1, "Live rendering initialized successfully");
    
    return true;
  }
  
  /**
   * Start live rendering
   * Begins real-time progressive rendering
   */
  void start()
  {
    if (!is_initialized_) {
      CLOG_ERROR(LOG_LILYBEAR, "Cannot start live rendering: not initialized");
      return;
    }
    
    if (is_active_) {
      CLOG_WARN(LOG_LILYBEAR, "Live rendering already active");
      return;
    }
    
    is_active_ = true;
    current_pass_ = 0;
    convergence_ratio_ = 0.0f;
    frame_count_ = 0;
    
    /* Reset timing */
    start_time_ = BLI_time_now_seconds();
    last_update_time_ = start_time_;
    frame_timer_ = 0.0;
    
    /* Clear accumulation buffer */
    GPU_framebuffer_bind(accumulation_framebuffer_);
    GPU_framebuffer_clear_color(accumulation_framebuffer_, float4(0.0f));
    GPU_framebuffer_restore();
    
    /* Start AI monitoring */
    if (ai_integration_) {
      ai_integration_->start_monitoring();
    }
    
    CLOG_INFO(LOG_LILYBEAR, 1, "Live rendering started");
  }
  
  /**
   * Stop live rendering
   * Stops real-time rendering and cleans up
   */
  void stop()
  {
    if (!is_active_) {
      return;
    }
    
    is_active_ = false;
    
    /* Stop AI monitoring */
    if (ai_integration_) {
      ai_integration_->stop_monitoring();
    }
    
    /* Update final statistics */
    update_statistics();
    
    CLOG_INFO(LOG_LILYBEAR, 1, "Live rendering stopped");
  }
  
  /**
   * Update live rendering
   * Called each frame to update progressive rendering
   */
  void update(const SceneData &scene_data, const RenderSettings &settings)
  {
    if (!is_active_ || !is_initialized_) {
      return;
    }
    
    const double current_time = BLI_time_now_seconds();
    const double delta_time = current_time - last_update_time_;
    last_update_time_ = current_time;
    
    /* Update frame timer */
    frame_timer_ += delta_time;
    frame_count_++;
    
    /* Update performance metrics */
    update_performance_metrics(delta_time);
    
    /* Adaptive quality adjustment */
    if (settings.enable_adaptive_quality) {
      adapt_quality_to_performance();
    }
    
    /* Progressive rendering pass */
    render_progressive_pass(scene_data, settings);
    
    /* AI analysis and suggestions */
    if (settings.enable_ai_integration && ai_integration_) {
      process_ai_analysis(scene_data, settings);
    }
    
    /* Update convergence ratio */
    update_convergence_ratio();
    
    /* Update statistics */
    update_statistics();
    
    /* Check if we should stop */
    if (should_stop_rendering(settings)) {
      stop();
    }
  }
  
  /**
   * Render progressive pass
   * Performs one iteration of progressive rendering
   */
  void render_progressive_pass(const SceneData &scene_data, const RenderSettings &settings)
  {
    /* Bind main framebuffer */
    GPU_framebuffer_bind(main_framebuffer_);
    
    /* Set up viewport */
    GPU_viewport(0, 0, resolution_.x, resolution_.y);
    
    /* Bind progressive shader */
    GPU_shader_bind(progressive_shader_);
    
    /* Update uniform buffers */
    update_settings_uniforms(settings);
    update_performance_uniforms();
    
    /* Bind textures */
    GPU_shader_bind_texture(progressive_shader_, "depth_tx", depth_texture_);
    GPU_shader_bind_texture(progressive_shader_, "normal_tx", normal_texture_);
    GPU_shader_bind_texture(progressive_shader_, "albedo_tx", albedo_texture_);
    GPU_shader_bind_texture(progressive_shader_, "roughness_tx", roughness_texture_);
    GPU_shader_bind_texture(progressive_shader_, "metallic_tx", metallic_texture_);
    
    /* Render full-screen quad */
    GPU_draw_fullscreen_quad();
    
    /* Unbind shader */
    GPU_shader_unbind();
    
    /* Accumulate result */
    accumulate_result();
    
    /* Increment pass counter */
    current_pass_++;
    
    /* Restore framebuffer */
    GPU_framebuffer_restore();
  }
  
  /**
   * Accumulate rendering result
   * Blends current pass with accumulated result
   */
  void accumulate_result()
  {
    /* Bind accumulation framebuffer */
    GPU_framebuffer_bind(accumulation_framebuffer_);
    
    /* Bind compositing shader */
    GPU_shader_bind(compositing_shader_);
    
    /* Bind current result texture */
    GPU_shader_bind_texture(compositing_shader_, "current_tx", color_texture_);
    
    /* Set accumulation parameters */
    GPU_shader_uniform_1i(compositing_shader_, "current_pass", current_pass_);
    GPU_shader_uniform_1f(compositing_shader_, "blend_factor", 1.0f / (current_pass_ + 1));
    
    /* Render accumulation */
    GPU_draw_fullscreen_quad();
    
    /* Unbind shader */
    GPU_shader_unbind();
    
    /* Restore framebuffer */
    GPU_framebuffer_restore();
  }
  
  /**
   * Update performance metrics
   * Measures and updates current performance data
   */
  void update_performance_metrics(double delta_time)
  {
    /* Update timing metrics */
    current_metrics_.frame_time = delta_time;
    current_metrics_.fps = 1.0f / delta_time;
    
    /* Update render time */
    current_metrics_.render_time = delta_time * 0.8f; /* Estimate 80% for rendering */
    
    /* Get GPU metrics */
    GPU_get_memory_usage(&current_metrics_.gpu_memory_used);
    GPU_get_utilization(&current_metrics_.gpu_utilization);
    
    /* Get system metrics */
    get_system_metrics(&current_metrics_);
    
    /* Update quality metrics */
    current_metrics_.current_quality = adaptive_quality_factor_;
    current_metrics_.target_quality = target_metrics_.target_quality;
    
    /* Update convergence */
    current_metrics_.convergence_ratio = convergence_ratio_;
  }
  
  /**
   * Adapt quality to performance
   * Adjusts rendering quality based on performance targets
   */
  void adapt_quality_to_performance()
  {
    const float target_fps = target_metrics_.fps;
    const float current_fps = current_metrics_.fps;
    const float fps_ratio = current_fps / target_fps;
    
    /* Adjust quality based on FPS */
    if (fps_ratio < 0.8f) {
      /* FPS too low, reduce quality */
      adaptive_quality_factor_ = std::max(adaptive_quality_factor_ * 0.95f, MIN_QUALITY_FACTOR);
    }
    else if (fps_ratio > 1.2f) {
      /* FPS high enough, increase quality */
      adaptive_quality_factor_ = std::min(adaptive_quality_factor_ * 1.05f, MAX_QUALITY_FACTOR);
    }
    
    /* Clamp to valid range */
    adaptive_quality_factor_ = std::clamp(adaptive_quality_factor_, MIN_QUALITY_FACTOR, MAX_QUALITY_FACTOR);
    
    /* Update quality factor */
    quality_factor_ = adaptive_quality_factor_;
  }
  
  /**
   * Process AI analysis
   * Performs AI analysis and generates suggestions
   */
  void process_ai_analysis(const SceneData &scene_data, const RenderSettings &settings)
  {
    if (!ai_integration_) {
      return;
    }
    
    /* Analyze scene complexity */
    const float complexity_score = calculate_scene_complexity(scene_data);
    
    /* Generate AI suggestions */
    ai_suggestions_.clear();
    ai_integration_->analyze_scene(scene_data, current_metrics_, ai_suggestions_);
    
    /* Apply AI optimizations if enabled */
    if (settings.enable_ai_optimization) {
      ai_integration_->apply_optimizations(scene_data, current_metrics_, settings);
    }
    
    /* Update AI metrics */
    render_stats_.ai_suggestions_generated = ai_suggestions_.size();
    render_stats_.ai_processing_time = ai_integration_->get_processing_time();
    render_stats_.ai_optimization_score = ai_integration_->get_optimization_score();
  }
  
  /**
   * Update convergence ratio
   * Calculates how close the rendering is to convergence
   */
  void update_convergence_ratio()
  {
    /* Simple convergence based on pass count */
    const float max_passes = 100.0f; /* Arbitrary maximum */
    convergence_ratio_ = std::min(current_pass_ / max_passes, 1.0f);
    
    /* More sophisticated convergence could analyze noise levels */
    // convergence_ratio_ = analyze_noise_levels();
  }
  
  /**
   * Check if rendering should stop
   * Determines if we've reached stopping criteria
   */
  bool should_stop_rendering(const RenderSettings &settings)
  {
    /* Check maximum render time */
    const double elapsed_time = BLI_time_now_seconds() - start_time_;
    if (elapsed_time > settings.max_render_time) {
      return true;
    }
    
    /* Check convergence */
    if (convergence_ratio_ > 0.95f) {
      return true;
    }
    
    /* Check maximum passes */
    if (current_pass_ >= settings.progressive_passes) {
      return true;
    }
    
    return false;
  }
  
  /**
   * Update statistics
   * Updates comprehensive render statistics
   */
  void update_statistics()
  {
    const double elapsed_time = BLI_time_now_seconds() - start_time_;
    
    render_stats_.total_time = elapsed_time;
    render_stats_.fps = current_metrics_.fps;
    render_stats_.frame_time_ms = current_metrics_.frame_time * 1000.0f;
    render_stats_.gpu_utilization = current_metrics_.gpu_utilization;
    render_stats_.cpu_utilization = current_metrics_.cpu_utilization;
    render_stats_.gpu_memory_used = current_metrics_.gpu_memory_used;
    render_stats_.system_memory_used = current_metrics_.system_memory_used;
    render_stats_.samples_per_pixel = current_pass_;
    render_stats_.convergence_ratio = convergence_ratio_;
    render_stats_.live_rendering_active = is_active_;
    render_stats_.live_updates = frame_count_;
    render_stats_.live_update_frequency = current_metrics_.fps;
  }
  
  /**
   * Get current result texture
   * Returns the current rendering result
   */
  GPUTexture *get_result_texture()
  {
    return color_texture_;
  }
  
  /**
   * Get performance metrics
   * Returns current performance data
   */
  const PerformanceMetrics &get_performance_metrics() const
  {
    return current_metrics_;
  }
  
  /**
   * Get render statistics
   * Returns comprehensive render statistics
   */
  const RenderStats &get_render_stats() const
  {
    return render_stats_;
  }
  
  /**
   * Get AI suggestions
   * Returns current AI-generated suggestions
   */
  const std::vector<AISuggestion> &get_ai_suggestions() const
  {
    return ai_suggestions_;
  }
  
  /**
   * Set quality factor
   * Manually sets rendering quality
   */
  void set_quality_factor(float factor)
  {
    quality_factor_ = std::clamp(factor, MIN_QUALITY_FACTOR, MAX_QUALITY_FACTOR);
    adaptive_quality_factor_ = quality_factor_;
  }
  
  /**
   * Set performance targets
   * Sets target performance goals
   */
  void set_performance_targets(const PerformanceMetrics &targets)
  {
    target_metrics_ = targets;
  }
  
 private:
  /**
   * Create GPU textures
   * Allocates texture resources for rendering
   */
  bool create_textures()
  {
    /* Create color texture */
    color_texture_ = GPU_texture_create_2d("lilybear_color",
                                          resolution_.x,
                                          resolution_.y,
                                          1,
                                          GPU_RGBA16F,
                                          GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_ATTACHMENT,
                                          nullptr);
    
    /* Create depth texture */
    depth_texture_ = GPU_texture_create_2d("lilybear_depth",
                                          resolution_.x,
                                          resolution_.y,
                                          1,
                                          GPU_DEPTH_COMPONENT24,
                                          GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_ATTACHMENT,
                                          nullptr);
    
    /* Create other textures */
    normal_texture_ = GPU_texture_create_2d("lilybear_normal",
                                           resolution_.x,
                                           resolution_.y,
                                           1,
                                           GPU_RGBA16F,
                                           GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_ATTACHMENT,
                                           nullptr);
    
    albedo_texture_ = GPU_texture_create_2d("lilybear_albedo",
                                           resolution_.x,
                                           resolution_.y,
                                           1,
                                           GPU_RGBA16F,
                                           GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_ATTACHMENT,
                                           nullptr);
    
    roughness_texture_ = GPU_texture_create_2d("lilybear_roughness",
                                              resolution_.x,
                                              resolution_.y,
                                              1,
                                              GPU_R16F,
                                              GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_ATTACHMENT,
                                              nullptr);
    
    metallic_texture_ = GPU_texture_create_2d("lilybear_metallic",
                                             resolution_.x,
                                             resolution_.y,
                                             1,
                                             GPU_R16F,
                                             GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_ATTACHMENT,
                                             nullptr);
    
    return (color_texture_ && depth_texture_ && normal_texture_ && albedo_texture_ &&
            roughness_texture_ && metallic_texture_);
  }
  
  /**
   * Create framebuffers
   * Allocates framebuffer resources
   */
  bool create_framebuffers()
  {
    /* Create main framebuffer */
    main_framebuffer_ = GPU_framebuffer_create("lilybear_main");
    GPU_framebuffer_texture_attach(main_framebuffer_, color_texture_, 0, 0);
    GPU_framebuffer_texture_attach(main_framebuffer_, depth_texture_, 0, 0);
    
    /* Create accumulation framebuffer */
    accumulation_framebuffer_ = GPU_framebuffer_create("lilybear_accumulation");
    GPU_framebuffer_texture_attach(accumulation_framebuffer_, color_texture_, 0, 0);
    
    return (main_framebuffer_ && accumulation_framebuffer_);
  }
  
  /**
   * Create shaders
   * Compiles and links shader programs
   */
  bool create_shaders()
  {
    /* TODO: Implement shader creation */
    /* For now, return true to allow compilation */
    return true;
  }
  
  /**
   * Create uniform buffers
   * Allocates uniform buffer objects
   */
  bool create_uniform_buffers()
  {
    /* Create settings uniform buffer */
    settings_ubo_ = GPU_uniformbuf_create_ex(sizeof(RenderSettings), nullptr, "lilybear_settings");
    
    /* Create performance uniform buffer */
    performance_ubo_ = GPU_uniformbuf_create_ex(sizeof(PerformanceMetrics),
                                               nullptr,
                                               "lilybear_performance");
    
    return (settings_ubo_ && performance_ubo_);
  }
  
  /**
   * Update settings uniforms
   * Updates uniform buffer with current settings
   */
  void update_settings_uniforms(const RenderSettings &settings)
  {
    if (settings_ubo_) {
      GPU_uniformbuf_update(settings_ubo_, &settings);
    }
  }
  
  /**
   * Update performance uniforms
   * Updates uniform buffer with current performance data
   */
  void update_performance_uniforms()
  {
    if (performance_ubo_) {
      GPU_uniformbuf_update(performance_ubo_, &current_metrics_);
    }
  }
  
  /**
   * Get system metrics
   * Retrieves system performance data
   */
  void get_system_metrics(PerformanceMetrics *metrics)
  {
    /* TODO: Implement system metrics retrieval */
    /* For now, set default values */
    metrics->cpu_utilization = 50.0f;
    metrics->system_memory_used = 1024 * 1024 * 1024; /* 1GB */
    metrics->temperature = 60.0f;
    metrics->power_consumption = 100.0f;
    metrics->fan_speed = 1500;
  }
  
  /**
   * Cleanup resources
   * Frees all allocated resources
   */
  void cleanup()
  {
    /* Free GPU textures */
    if (color_texture_) {
      GPU_texture_free(color_texture_);
      color_texture_ = nullptr;
    }
    if (depth_texture_) {
      GPU_texture_free(depth_texture_);
      depth_texture_ = nullptr;
    }
    if (normal_texture_) {
      GPU_texture_free(normal_texture_);
      normal_texture_ = nullptr;
    }
    if (albedo_texture_) {
      GPU_texture_free(albedo_texture_);
      albedo_texture_ = nullptr;
    }
    if (roughness_texture_) {
      GPU_texture_free(roughness_texture_);
      roughness_texture_ = nullptr;
    }
    if (metallic_texture_) {
      GPU_texture_free(metallic_texture_);
      metallic_texture_ = nullptr;
    }
    
    /* Free framebuffers */
    if (main_framebuffer_) {
      GPU_framebuffer_free(main_framebuffer_);
      main_framebuffer_ = nullptr;
    }
    if (accumulation_framebuffer_) {
      GPU_framebuffer_free(accumulation_framebuffer_);
      accumulation_framebuffer_ = nullptr;
    }
    
    /* Free shaders */
    if (progressive_shader_) {
      GPU_shader_free(progressive_shader_);
      progressive_shader_ = nullptr;
    }
    if (denoising_shader_) {
      GPU_shader_free(denoising_shader_);
      denoising_shader_ = nullptr;
    }
    if (compositing_shader_) {
      GPU_shader_free(compositing_shader_);
      compositing_shader_ = nullptr;
    }
    
    /* Free uniform buffers */
    if (settings_ubo_) {
      GPU_uniformbuf_free(settings_ubo_);
      settings_ubo_ = nullptr;
    }
    if (performance_ubo_) {
      GPU_uniformbuf_free(performance_ubo_);
      performance_ubo_ = nullptr;
    }
    
    /* Free AI integration */
    if (ai_integration_) {
      delete ai_integration_;
      ai_integration_ = nullptr;
    }
    
    is_initialized_ = false;
    is_active_ = false;
  }
};

/* -------------------------------------------------------------------- */
/** \name Global Live Rendering Manager
 * \{ */

/**
 * Global live rendering manager
 * Manages all live rendering instances
 */
class LiveRenderingManager {
 private:
  static LiveRenderingManager *instance_;
  std::map<RenderEngine *, LiveRenderingInstance *> instances_;
  bool is_initialized_;
  
 public:
  LiveRenderingManager() : is_initialized_(false) {}
  
  static LiveRenderingManager *get_instance()
  {
    if (!instance_) {
      instance_ = new LiveRenderingManager();
    }
    return instance_;
  }
  
  /**
   * Initialize live rendering system
   */
  bool init()
  {
    if (is_initialized_) {
      return true;
    }
    
    is_initialized_ = true;
    CLOG_INFO(LOG_LILYBEAR, 1, "Live rendering manager initialized");
    return true;
  }
  
  /**
   * Cleanup live rendering system
   */
  void cleanup()
  {
    /* Cleanup all instances */
    for (auto &pair : instances_) {
      delete pair.second;
    }
    instances_.clear();
    
    is_initialized_ = false;
    CLOG_INFO(LOG_LILYBEAR, 1, "Live rendering manager cleaned up");
  }
  
  /**
   * Create live rendering instance
   */
  LiveRenderingInstance *create_instance(RenderEngine *engine,
                                        const int2 &resolution,
                                        const RenderSettings &settings)
  {
    if (!is_initialized_) {
      return nullptr;
    }
    
    /* Check if instance already exists */
    auto it = instances_.find(engine);
    if (it != instances_.end()) {
      return it->second;
    }
    
    /* Create new instance */
    LiveRenderingInstance *instance = new LiveRenderingInstance();
    if (!instance->init(resolution, settings)) {
      delete instance;
      return nullptr;
    }
    
    instances_[engine] = instance;
    return instance;
  }
  
  /**
   * Get live rendering instance
   */
  LiveRenderingInstance *get_instance(RenderEngine *engine)
  {
    auto it = instances_.find(engine);
    return (it != instances_.end()) ? it->second : nullptr;
  }
  
  /**
   * Destroy live rendering instance
   */
  void destroy_instance(RenderEngine *engine)
  {
    auto it = instances_.find(engine);
    if (it != instances_.end()) {
      delete it->second;
      instances_.erase(it);
    }
  }
};

/* Initialize static instance */
LiveRenderingManager *LiveRenderingManager::instance_ = nullptr;

/* -------------------------------------------------------------------- */
/** \name Public Interface
 * \{ */

/**
 * Initialize live rendering system
 */
void init()
{
  LiveRenderingManager::get_instance()->init();
}

/**
 * Cleanup live rendering system
 */
void cleanup()
{
  LiveRenderingManager::get_instance()->cleanup();
}

/**
 * Start live rendering for engine
 */
void start_live_rendering(RenderEngine *engine, const int2 &resolution, const RenderSettings &settings)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->create_instance(engine, resolution, settings);
  
  if (instance) {
    instance->start();
  }
}

/**
 * Stop live rendering for engine
 */
void stop_live_rendering(RenderEngine *engine)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->stop();
  }
}

/**
 * Update live rendering for engine
 */
void update_live_rendering(RenderEngine *engine, const SceneData &scene_data, const RenderSettings &settings)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->update(scene_data, settings);
  }
}

/**
 * Get live rendering result texture
 */
GPUTexture *get_live_rendering_result(RenderEngine *engine)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->get_instance(engine);
  
  return instance ? instance->get_result_texture() : nullptr;
}

/**
 * Get live rendering performance metrics
 */
PerformanceMetrics get_live_rendering_performance(RenderEngine *engine)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->get_instance(engine);
  
  return instance ? instance->get_performance_metrics() : PerformanceMetrics{};
}

/**
 * Get live rendering statistics
 */
RenderStats get_live_rendering_stats(RenderEngine *engine)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->get_instance(engine);
  
  return instance ? instance->get_render_stats() : RenderStats{};
}

/**
 * Get live rendering AI suggestions
 */
std::vector<AISuggestion> get_live_rendering_ai_suggestions(RenderEngine *engine)
{
  LiveRenderingManager *manager = LiveRenderingManager::get_instance();
  LiveRenderingInstance *instance = manager->get_instance(engine);
  
  return instance ? instance->get_ai_suggestions() : std::vector<AISuggestion>{};
}

}  // namespace blender::lilybear 