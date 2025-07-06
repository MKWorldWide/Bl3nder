/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "BLI_math_vector.hh"
#include "BLI_string.h"
#include "BLI_utildefines.h"

#include "GPU_shader.hh"
#include "GPU_uniform_buffer.hh"

#include "lilybear_types.hh"

namespace blender::lilybear {

/**
 * 🎨 Lilybear Shader Management
 * 
 * Manages shaders for the Lilybear render engine:
 * - Progressive rendering shaders
 * - AI denoising shaders
 * - Compositing shaders
 * - Utility shaders
 */

/* -------------------------------------------------------------------- */
/** \name Shader Module
 * \{ */

/**
 * Shader Module
 * Manages all shaders for the Lilybear engine
 */
class ShaderModule {
 private:
  /* Core Shaders */
  GPUShader *progressive_shader_;
  GPUShader *denoising_shader_;
  GPUShader *compositing_shader_;
  GPUShader *utility_shader_;
  
  /* Uniform Buffers */
  GPUUniformBuf *settings_ubo_;
  GPUUniformBuf *performance_ubo_;
  GPUUniformBuf *scene_ubo_;
  
  /* Shader State */
  bool is_initialized_;
  
 public:
  ShaderModule();
  ~ShaderModule();
  
  /**
   * Initialize shader module
   * Creates all required shaders
   */
  bool init();
  
  /**
   * Get progressive shader
   * Returns the progressive rendering shader
   */
  GPUShader *get_progressive_shader() const;
  
  /**
   * Get denoising shader
   * Returns the AI denoising shader
   */
  GPUShader *get_denoising_shader() const;
  
  /**
   * Get compositing shader
   * Returns the compositing shader
   */
  GPUShader *get_compositing_shader() const;
  
  /**
   * Get utility shader
   * Returns the utility shader
   */
  GPUShader *get_utility_shader() const;
  
  /**
   * Get settings uniform buffer
   * Returns the settings uniform buffer
   */
  GPUUniformBuf *get_settings_ubo() const;
  
  /**
   * Get performance uniform buffer
   * Returns the performance uniform buffer
   */
  GPUUniformBuf *get_performance_ubo() const;
  
  /**
   * Get scene uniform buffer
   * Returns the scene uniform buffer
   */
  GPUUniformBuf *get_scene_ubo() const;
  
  /**
   * Update settings uniform buffer
   * Updates the settings uniform buffer with new data
   */
  void update_settings_ubo(const RenderSettings &settings);
  
  /**
   * Update performance uniform buffer
   * Updates the performance uniform buffer with new data
   */
  void update_performance_ubo(const PerformanceMetrics &metrics);
  
  /**
   * Update scene uniform buffer
   * Updates the scene uniform buffer with new data
   */
  void update_scene_ubo(const SceneData &scene_data);
  
  /**
   * Is initialized
   * Returns whether the shader module is initialized
   */
  bool is_initialized() const;
  
  /**
   * Free shader module
   * Frees all shader resources
   */
  void module_free();
  
 private:
  /**
   * Create progressive shader
   * Creates the progressive rendering shader
   */
  bool create_progressive_shader();
  
  /**
   * Create denoising shader
   * Creates the AI denoising shader
   */
  bool create_denoising_shader();
  
  /**
   * Create compositing shader
   * Creates the compositing shader
   */
  bool create_compositing_shader();
  
  /**
   * Create utility shader
   * Creates the utility shader
   */
  bool create_utility_shader();
  
  /**
   * Create uniform buffers
   * Creates all uniform buffers
   */
  bool create_uniform_buffers();
  
  /**
   * Cleanup shaders
   * Frees all shader resources
   */
  void cleanup_shaders();
};

/* -------------------------------------------------------------------- */
/** \name Shader Module Implementation
 * \{ */

ShaderModule::ShaderModule() : progressive_shader_(nullptr),
                               denoising_shader_(nullptr),
                               compositing_shader_(nullptr),
                               utility_shader_(nullptr),
                               settings_ubo_(nullptr),
                               performance_ubo_(nullptr),
                               scene_ubo_(nullptr),
                               is_initialized_(false)
{
}

ShaderModule::~ShaderModule()
{
  module_free();
}

bool ShaderModule::init()
{
  if (is_initialized_) {
    return true;
  }
  
  /* Create progressive shader */
  if (!create_progressive_shader()) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create progressive shader");
    return false;
  }
  
  /* Create denoising shader */
  if (!create_denoising_shader()) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create denoising shader");
    return false;
  }
  
  /* Create compositing shader */
  if (!create_compositing_shader()) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create compositing shader");
    return false;
  }
  
  /* Create utility shader */
  if (!create_utility_shader()) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create utility shader");
    return false;
  }
  
  /* Create uniform buffers */
  if (!create_uniform_buffers()) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create uniform buffers");
    return false;
  }
  
  is_initialized_ = true;
  CLOG_INFO(LOG_LILYBEAR, 1, "Shader module initialized successfully");
  
  return true;
}

GPUShader *ShaderModule::get_progressive_shader() const
{
  return progressive_shader_;
}

GPUShader *ShaderModule::get_denoising_shader() const
{
  return denoising_shader_;
}

GPUShader *ShaderModule::get_compositing_shader() const
{
  return compositing_shader_;
}

GPUShader *ShaderModule::get_utility_shader() const
{
  return utility_shader_;
}

GPUUniformBuf *ShaderModule::get_settings_ubo() const
{
  return settings_ubo_;
}

GPUUniformBuf *ShaderModule::get_performance_ubo() const
{
  return performance_ubo_;
}

GPUUniformBuf *ShaderModule::get_scene_ubo() const
{
  return scene_ubo_;
}

void ShaderModule::update_settings_ubo(const RenderSettings &settings)
{
  if (settings_ubo_) {
    GPU_uniformbuf_update(settings_ubo_, &settings);
  }
}

void ShaderModule::update_performance_ubo(const PerformanceMetrics &metrics)
{
  if (performance_ubo_) {
    GPU_uniformbuf_update(performance_ubo_, &metrics);
  }
}

void ShaderModule::update_scene_ubo(const SceneData &scene_data)
{
  if (scene_ubo_) {
    GPU_uniformbuf_update(scene_ubo_, &scene_data);
  }
}

bool ShaderModule::is_initialized() const
{
  return is_initialized_;
}

void ShaderModule::module_free()
{
  if (!is_initialized_) {
    return;
  }
  
  cleanup_shaders();
  is_initialized_ = false;
  CLOG_INFO(LOG_LILYBEAR, 1, "Shader module freed");
}

/* -------------------------------------------------------------------- */
/** \name Private Implementation
 * \{ */

bool ShaderModule::create_progressive_shader()
{
  /* Create progressive rendering shader */
  progressive_shader_ = GPU_shader_create_from_info_name("lilybear_progressive");
  
  if (!progressive_shader_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create progressive shader");
    return false;
  }
  
  CLOG_INFO(LOG_LILYBEAR, 2, "Progressive shader created successfully");
  return true;
}

bool ShaderModule::create_denoising_shader()
{
  /* Create AI denoising shader */
  denoising_shader_ = GPU_shader_create_from_info_name("lilybear_denoising");
  
  if (!denoising_shader_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create denoising shader");
    return false;
  }
  
  CLOG_INFO(LOG_LILYBEAR, 2, "Denoising shader created successfully");
  return true;
}

bool ShaderModule::create_compositing_shader()
{
  /* Create compositing shader */
  compositing_shader_ = GPU_shader_create_from_info_name("lilybear_compositing");
  
  if (!compositing_shader_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create compositing shader");
    return false;
  }
  
  CLOG_INFO(LOG_LILYBEAR, 2, "Compositing shader created successfully");
  return true;
}

bool ShaderModule::create_utility_shader()
{
  /* Create utility shader */
  utility_shader_ = GPU_shader_create_from_info_name("lilybear_utility");
  
  if (!utility_shader_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create utility shader");
    return false;
  }
  
  CLOG_INFO(LOG_LILYBEAR, 2, "Utility shader created successfully");
  return true;
}

bool ShaderModule::create_uniform_buffers()
{
  /* Create settings uniform buffer */
  settings_ubo_ = GPU_uniformbuf_create(sizeof(RenderSettings));
  if (!settings_ubo_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create settings uniform buffer");
    return false;
  }
  
  /* Create performance uniform buffer */
  performance_ubo_ = GPU_uniformbuf_create(sizeof(PerformanceMetrics));
  if (!performance_ubo_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create performance uniform buffer");
    return false;
  }
  
  /* Create scene uniform buffer */
  scene_ubo_ = GPU_uniformbuf_create(sizeof(SceneData));
  if (!scene_ubo_) {
    CLOG_ERROR(LOG_LILYBEAR, "Failed to create scene uniform buffer");
    return false;
  }
  
  CLOG_INFO(LOG_LILYBEAR, 2, "Uniform buffers created successfully");
  return true;
}

void ShaderModule::cleanup_shaders()
{
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
  if (utility_shader_) {
    GPU_shader_free(utility_shader_);
    utility_shader_ = nullptr;
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
  if (scene_ubo_) {
    GPU_uniformbuf_free(scene_ubo_);
    scene_ubo_ = nullptr;
  }
  
  CLOG_INFO(LOG_LILYBEAR, 2, "Shaders cleaned up");
}

/* -------------------------------------------------------------------- */
/** \name Global Shader Module Instance
 * \{ */

static ShaderModule *g_shader_module = nullptr;

/**
 * Get shader module
 * Returns the global shader module instance
 */
ShaderModule *get_shader_module()
{
  if (!g_shader_module) {
    g_shader_module = new ShaderModule();
    if (!g_shader_module->init()) {
      CLOG_ERROR(LOG_LILYBEAR, "Failed to initialize global shader module");
      delete g_shader_module;
      g_shader_module = nullptr;
    }
  }
  return g_shader_module;
}

/**
 * Free shader module
 * Frees the global shader module instance
 */
void free_shader_module()
{
  if (g_shader_module) {
    g_shader_module->module_free();
    delete g_shader_module;
    g_shader_module = nullptr;
  }
}

}  // namespace blender::lilybear 