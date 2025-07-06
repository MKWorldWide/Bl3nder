/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "BLI_math_vector.hh"
#include "BLI_string.h"
#include "BLI_time.h"
#include "BLI_utildefines.h"

#include "lilybear_types.hh"

namespace blender::lilybear {

/**
 * 🛠️ Lilybear Utility Functions
 * 
 * Provides utility functions for the Lilybear render engine:
 * - Quality level conversion
 * - Settings validation
 * - Performance calculations
 * - Logging utilities
 */

/* -------------------------------------------------------------------- */
/** \name Quality Level Functions
 * \{ */

/**
 * Convert quality level to factor
 * Converts a quality level enum to a quality factor
 */
float lilybear_quality_level_to_factor(LilybearQualityLevel level)
{
  switch (level) {
    case LILYBEAR_QUALITY_DRAFT:
      return 0.25f;
    case LILYBEAR_QUALITY_PREVIEW:
      return 0.5f;
    case LILYBEAR_QUALITY_GOOD:
      return 0.75f;
    case LILYBEAR_QUALITY_HIGH:
      return 1.0f;
    case LILYBEAR_QUALITY_PRODUCTION:
      return 1.5f;
    default:
      return 1.0f;
  }
}

/**
 * Convert factor to quality level
 * Converts a quality factor to a quality level enum
 */
LilybearQualityLevel lilybear_factor_to_quality_level(float factor)
{
  if (factor <= 0.3f) {
    return LILYBEAR_QUALITY_DRAFT;
  } else if (factor <= 0.6f) {
    return LILYBEAR_QUALITY_PREVIEW;
  } else if (factor <= 0.85f) {
    return LILYBEAR_QUALITY_GOOD;
  } else if (factor <= 1.2f) {
    return LILYBEAR_QUALITY_HIGH;
  } else {
    return LILYBEAR_QUALITY_PRODUCTION;
  }
}

/* -------------------------------------------------------------------- */
/** \name Settings Functions
 * \{ */

/**
 * Get default Lilybear settings
 * Returns default render settings for optimal performance
 */
LilybearRenderSettings get_default_lilybear_settings()
{
  LilybearRenderSettings settings = {};
  
  /* Quality Settings */
  settings.quality_level = LILYBEAR_QUALITY_HIGH;
  settings.base_samples = 16;
  settings.quality_factor = 1.0f;
  
  /* Live Rendering Settings */
  settings.enable_live_rendering = true;
  settings.live_update_rate = 30.0f;
  settings.progressive_passes = 16;
  
  /* AI Settings */
  settings.enable_ai_integration = true;
  settings.enable_ai_suggestions = true;
  settings.enable_ai_optimization = true;
  settings.ai_confidence_threshold = 0.7f;
  
  /* Performance Settings */
  settings.enable_adaptive_quality = true;
  settings.enable_performance_monitoring = true;
  settings.performance_target_fps = 30.0f;
  
  /* Advanced Settings */
  settings.enable_intelligent_sampling = true;
  settings.enable_real_time_denoising = true;
  settings.enable_workflow_automation = true;
  
  return settings;
}

/**
 * Validate Lilybear settings
 * Validates render settings for consistency and correctness
 */
bool validate_lilybear_settings(const LilybearRenderSettings &settings)
{
  /* Validate quality settings */
  if (settings.quality_factor < 0.1f || settings.quality_factor > 2.0f) {
    CLOG_ERROR(LOG_LILYBEAR, "Invalid quality factor: %.2f", settings.quality_factor);
    return false;
  }
  
  if (settings.base_samples < 1 || settings.base_samples > 1024) {
    CLOG_ERROR(LOG_LILYBEAR, "Invalid base samples: %d", settings.base_samples);
    return false;
  }
  
  /* Validate live rendering settings */
  if (settings.live_update_rate < 1.0f || settings.live_update_rate > 120.0f) {
    CLOG_ERROR(LOG_LILYBEAR, "Invalid live update rate: %.2f", settings.live_update_rate);
    return false;
  }
  
  if (settings.progressive_passes < 1 || settings.progressive_passes > 256) {
    CLOG_ERROR(LOG_LILYBEAR, "Invalid progressive passes: %d", settings.progressive_passes);
    return false;
  }
  
  /* Validate AI settings */
  if (settings.ai_confidence_threshold < 0.0f || settings.ai_confidence_threshold > 1.0f) {
    CLOG_ERROR(LOG_LILYBEAR, "Invalid AI confidence threshold: %.2f", settings.ai_confidence_threshold);
    return false;
  }
  
  /* Validate performance settings */
  if (settings.performance_target_fps < 1.0f || settings.performance_target_fps > 120.0f) {
    CLOG_ERROR(LOG_LILYBEAR, "Invalid performance target FPS: %.2f", settings.performance_target_fps);
    return false;
  }
  
  return true;
}

/* -------------------------------------------------------------------- */
/** \name Performance Functions
 * \{ */

/**
 * Calculate performance score
 * Calculates a performance score based on metrics
 */
float calculate_performance_score(const PerformanceMetrics &metrics)
{
  float score = 0.0f;
  
  /* FPS score (0-40 points) */
  score += MIN(metrics.fps / 60.0f, 1.0f) * 40.0f;
  
  /* Frame time score (0-20 points) */
  score += MAX(0.0f, 1.0f - metrics.frame_time / 33.33f) * 20.0f;
  
  /* GPU utilization score (0-20 points) */
  score += MAX(0.0f, 1.0f - metrics.gpu_utilization / 100.0f) * 20.0f;
  
  /* Memory usage score (0-20 points) */
  score += MAX(0.0f, 1.0f - metrics.memory_usage / 4096.0f) * 20.0f;
  
  return score;
}

/**
 * Calculate quality score
 * Calculates a quality score based on metrics
 */
float calculate_quality_score(const PerformanceMetrics &metrics)
{
  float score = 0.0f;
  
  /* Samples per pixel score (0-30 points) */
  score += MIN(metrics.samples_per_pixel / 64.0f, 1.0f) * 30.0f;
  
  /* Noise level score (0-40 points) */
  score += MAX(0.0f, 1.0f - metrics.noise_level) * 40.0f;
  
  /* AI optimization score (0-30 points) */
  score += metrics.ai_optimization_score * 30.0f;
  
  return score;
}

/**
 * Calculate overall score
 * Calculates an overall score balancing performance and quality
 */
float calculate_overall_score(const PerformanceMetrics &metrics)
{
  float performance_score = calculate_performance_score(metrics);
  float quality_score = calculate_quality_score(metrics);
  
  /* Balance performance and quality (60% performance, 40% quality) */
  return performance_score * 0.6f + quality_score * 0.4f;
}

/* -------------------------------------------------------------------- */
/** \name Logging Functions
 * \{ */

/**
 * Initialize Lilybear logging
 * Sets up logging for the Lilybear engine
 */
void init_lilybear_logging()
{
  /* Initialize logging category */
  CLOG_INFO(LOG_LILYBEAR, 1, "Lilybear logging initialized");
}

/**
 * Set Lilybear log level
 * Sets the logging level for Lilybear messages
 */
void set_lilybear_log_level(int level)
{
  /* Set logging level */
  CLOG_INFO(LOG_LILYBEAR, 1, "Lilybear log level set to %d", level);
}

/**
 * Log Lilybear performance
 * Logs performance metrics for monitoring
 */
void log_lilybear_performance(const PerformanceMetrics &metrics)
{
  CLOG_INFO(LOG_LILYBEAR, 2, 
            "Performance: FPS=%.1f, FrameTime=%.2fms, GPU=%.1f%%, Memory=%.1fMB, "
            "Samples=%d, Noise=%.3f, AI_Score=%.2f",
            metrics.fps, metrics.frame_time, metrics.gpu_utilization, 
            metrics.memory_usage, metrics.samples_per_pixel, metrics.noise_level,
            metrics.ai_optimization_score);
}

/**
 * Log Lilybear AI suggestion
 * Logs AI suggestions for debugging
 */
void log_lilybear_ai_suggestion(const AISuggestion &suggestion)
{
  CLOG_INFO(LOG_LILYBEAR, 2, 
            "AI Suggestion: Type=%d, Title='%s', Confidence=%.2f, Impact=%.2f",
            suggestion.type, suggestion.title, suggestion.confidence, suggestion.impact_score);
}

/* -------------------------------------------------------------------- */
/** \name Utility Functions
 * \{ */

/**
 * Calculate optimal samples per pixel
 * Calculates optimal samples based on scene complexity and performance
 */
int calculate_optimal_samples(float scene_complexity, float performance_target)
{
  /* Base samples calculation */
  int base_samples = int(16.0f * scene_complexity);
  
  /* Adjust for performance target */
  float performance_factor = CLAMP(performance_target / 30.0f, 0.5f, 2.0f);
  int optimal_samples = int(base_samples * performance_factor);
  
  /* Clamp to reasonable range */
  return CLAMP(optimal_samples, 1, 256);
}

/**
 * Calculate adaptive quality factor
 * Calculates adaptive quality factor based on performance
 */
float calculate_adaptive_quality_factor(const PerformanceMetrics &current,
                                       const PerformanceTargets &targets)
{
  float quality_factor = 1.0f;
  
  /* Adjust based on frame time */
  if (current.frame_time > targets.max_frame_time) {
    quality_factor *= 0.9f;
  } else if (current.frame_time < targets.max_frame_time * 0.8f) {
    quality_factor *= 1.1f;
  }
  
  /* Adjust based on GPU utilization */
  if (current.gpu_utilization > targets.max_gpu_utilization) {
    quality_factor *= 0.95f;
  }
  
  /* Adjust based on memory usage */
  if (current.memory_usage > targets.max_memory_usage) {
    quality_factor *= 0.9f;
  }
  
  /* Clamp to target range */
  return CLAMP(quality_factor, targets.min_quality, targets.target_quality);
}

/**
 * Estimate render time
 * Estimates render time based on scene complexity and settings
 */
double estimate_render_time(float scene_complexity,
                           const LilybearRenderSettings &settings,
                           const PerformanceMetrics &performance)
{
  /* Base render time calculation */
  double base_time = scene_complexity * settings.base_samples * 0.001; // seconds per sample
  
  /* Adjust for quality factor */
  base_time *= settings.quality_factor;
  
  /* Adjust for performance */
  double performance_factor = 30.0f / performance.fps;
  base_time *= performance_factor;
  
  /* Add AI processing time */
  base_time += performance.ai_processing_time * 0.001;
  
  return base_time;
}

/**
 * Calculate scene complexity
 * Calculates scene complexity score
 */
float calculate_scene_complexity(const SceneData &scene_data)
{
  float complexity = 0.0f;
  
  /* Object count factor */
  complexity += scene_data.object_count * 0.1f;
  
  /* Triangle count factor */
  complexity += scene_data.triangle_count * 0.0001f;
  
  /* Light count factor */
  complexity += scene_data.light_count * 0.5f;
  
  /* Material count factor */
  complexity += scene_data.material_count * 0.2f;
  
  /* Texture count factor */
  complexity += scene_data.texture_count * 0.1f;
  
  /* Clamp to reasonable range */
  return CLAMP(complexity, 0.1f, 10.0f);
}

/**
 * Format performance metrics
 * Formats performance metrics for display
 */
void format_performance_metrics(const PerformanceMetrics &metrics, char *buffer, size_t buffer_size)
{
  snprintf(buffer, buffer_size,
           "FPS: %.1f | Frame: %.1fms | GPU: %.1f%% | Memory: %.0fMB | "
           "Samples: %d | Noise: %.3f | AI: %.2f",
           metrics.fps, metrics.frame_time, metrics.gpu_utilization,
           metrics.memory_usage, metrics.samples_per_pixel, metrics.noise_level,
           metrics.ai_optimization_score);
}

/**
 * Format AI suggestion
 * Formats AI suggestion for display
 */
void format_ai_suggestion(const AISuggestion &suggestion, char *buffer, size_t buffer_size)
{
  snprintf(buffer, buffer_size,
           "[%.0f%%] %s: %s (Impact: %.2f)",
           suggestion.confidence * 100.0f, suggestion.title,
           suggestion.description, suggestion.impact_score);
}

}  // namespace blender::lilybear 