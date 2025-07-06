/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_sys_types.h"

#include "DRW_render.hh"

#include "lilybear_types.hh"

namespace blender::lilybear {

/**
 * 🚀 Lilybear Render Engine
 * 
 * Advanced render engine with AI-powered live rendering capabilities.
 * Provides real-time progressive rendering with intelligent optimization.
 */

class Engine {
 public:
  /**
   * Create a new Lilybear render instance
   * Returns pointer to the created instance
   */
  static DrawEngine *create_instance();

  /**
   * Free static resources used by the engine
   * Called during engine shutdown
   */
  static void free_static();
};

}  // namespace blender::lilybear

/* -------------------------------------------------------------------- */
/** \name Engine Registration
 * \{ */

/**
 * Register Lilybear render engine with Blender
 * Called during Blender initialization
 */
void register_lilybear_engine();

/**
 * Unregister Lilybear render engine from Blender
 * Called during Blender shutdown
 */
void unregister_lilybear_engine();

/* -------------------------------------------------------------------- */
/** \name Engine Capabilities
 * \{ */

/**
 * Check if Lilybear engine is available
 * Returns true if all required components are loaded
 */
bool is_lilybear_available();

/**
 * Get Lilybear engine capabilities
 * Returns bitfield of supported features
 */
uint32_t get_lilybear_capabilities();

/**
 * Initialize Lilybear engine settings
 * Sets up default configuration for optimal performance
 */
void init_lilybear_settings();

/* -------------------------------------------------------------------- */
/** \name AI Integration Interface
 * \{ */

/**
 * Process AI suggestions for scene optimization
 * Analyzes current scene and provides intelligent recommendations
 */
void process_ai_suggestions(RenderEngine *engine, Depsgraph *depsgraph);

/**
 * Apply AI optimizations to render settings
 * Automatically adjusts settings for optimal quality/performance balance
 */
void apply_ai_optimizations(RenderEngine *engine, Scene *scene);

/* -------------------------------------------------------------------- */
/** \name Live Rendering Interface
 * \{ */

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

/* -------------------------------------------------------------------- */
/** \name Performance Monitoring
 * \{ */

/**
 * Get current rendering performance metrics
 * Returns performance data for monitoring and optimization
 */
LilybearPerformanceMetrics get_performance_metrics(RenderEngine *engine);

/**
 * Set performance targets for adaptive rendering
 * Configures quality/performance balance based on user preferences
 */
void set_performance_targets(RenderEngine *engine, const LilybearPerformanceTargets &targets);

/* -------------------------------------------------------------------- */
/** \name Render Engine Type
 * \{ */

/**
 * Lilybear Render Engine Type
 * Advanced render engine with live rendering and AI capabilities
 */
extern RenderEngineType DRW_engine_viewport_lilybear_type;

/* -------------------------------------------------------------------- */
/** \name Constants and Enums
 * \{ */

/**
 * Lilybear Engine Capabilities
 * Bitfield flags for supported features
 */
enum LilybearCapabilities {
  LILYBEAR_CAP_LIVE_RENDERING = (1 << 0),
  LILYBEAR_CAP_AI_INTEGRATION = (1 << 1),
  LILYBEAR_CAP_PROGRESSIVE_RENDERING = (1 << 2),
  LILYBEAR_CAP_ADAPTIVE_QUALITY = (1 << 3),
  LILYBEAR_CAP_REAL_TIME_FEEDBACK = (1 << 4),
  LILYBEAR_CAP_AI_OPTIMIZATION = (1 << 5),
  LILYBEAR_CAP_PERFORMANCE_MONITORING = (1 << 6),
  LILYBEAR_CAP_INTELLIGENT_SAMPLING = (1 << 7),
};

/**
 * Lilybear Render Quality Levels
 * Defines different quality presets for adaptive rendering
 */
enum LilybearQualityLevel {
  LILYBEAR_QUALITY_DRAFT = 0,
  LILYBEAR_QUALITY_PREVIEW = 1,
  LILYBEAR_QUALITY_GOOD = 2,
  LILYBEAR_QUALITY_HIGH = 3,
  LILYBEAR_QUALITY_PRODUCTION = 4,
};

/**
 * Lilybear AI Suggestion Types
 * Different types of AI-generated suggestions
 */
enum LilybearAISuggestionType {
  LILYBEAR_AI_SUGGESTION_PERFORMANCE = 0,
  LILYBEAR_AI_SUGGESTION_QUALITY = 1,
  LILYBEAR_AI_SUGGESTION_WORKFLOW = 2,
  LILYBEAR_AI_SUGGESTION_CREATIVE = 3,
  LILYBEAR_AI_SUGGESTION_TECHNICAL = 4,
};

/* -------------------------------------------------------------------- */
/** \name Data Structures
 * \{ */

/**
 * Lilybear Performance Metrics
 * Real-time performance data for monitoring and optimization
 */
struct LilybearPerformanceMetrics {
  /* Rendering Performance */
  float fps;                    /* Current frames per second */
  float frame_time_ms;          /* Average frame time in milliseconds */
  float render_time_ms;         /* Time spent on rendering per frame */
  float gpu_utilization;        /* GPU utilization percentage */
  float memory_usage_mb;        /* GPU memory usage in MB */
  
  /* Quality Metrics */
  float current_quality;        /* Current rendering quality (0.0 - 1.0) */
  float target_quality;         /* Target quality level */
  int samples_per_pixel;        /* Current samples per pixel */
  float noise_level;            /* Current noise level estimate */
  
  /* AI Metrics */
  float ai_processing_time_ms;  /* Time spent on AI processing */
  int ai_suggestions_count;     /* Number of active AI suggestions */
  float ai_optimization_score;  /* AI optimization effectiveness score */
  
  /* Live Rendering Metrics */
  bool live_rendering_active;   /* Whether live rendering is active */
  float live_update_frequency;  /* Live update frequency in Hz */
  int progressive_passes;       /* Number of progressive passes completed */
  
  /* System Metrics */
  float cpu_utilization;        /* CPU utilization percentage */
  float system_memory_usage_mb; /* System memory usage in MB */
  float temperature_celsius;    /* GPU temperature in Celsius */
};

/**
 * Lilybear Performance Targets
 * Target performance goals for adaptive rendering
 */
struct LilybearPerformanceTargets {
  /* Performance Targets */
  float target_fps;             /* Target frames per second */
  float max_frame_time_ms;      /* Maximum acceptable frame time */
  float max_gpu_utilization;    /* Maximum GPU utilization */
  float max_memory_usage_mb;    /* Maximum memory usage */
  
  /* Quality Targets */
  float min_quality;            /* Minimum acceptable quality */
  float target_quality;         /* Target quality level */
  float max_noise_level;        /* Maximum acceptable noise level */
  
  /* AI Targets */
  float max_ai_processing_time_ms; /* Maximum AI processing time */
  int max_ai_suggestions;       /* Maximum number of AI suggestions */
  
  /* Adaptive Behavior */
  bool adaptive_quality;        /* Enable adaptive quality adjustment */
  bool adaptive_sampling;       /* Enable adaptive sampling */
  bool intelligent_optimization; /* Enable AI-driven optimization */
};

/**
 * Lilybear AI Suggestion
 * Individual AI-generated suggestion for scene optimization
 */
struct LilybearAISuggestion {
  LilybearAISuggestionType type; /* Type of suggestion */
  char title[256];               /* Suggestion title */
  char description[1024];        /* Detailed description */
  char action[512];              /* Recommended action */
  float confidence;              /* AI confidence level (0.0 - 1.0) */
  float impact_score;            /* Expected impact on performance/quality */
  bool applied;                  /* Whether suggestion has been applied */
  bool dismissed;                /* Whether suggestion has been dismissed */
};

/**
 * Lilybear Render Settings
 * Configuration settings for the Lilybear render engine
 */
struct LilybearRenderSettings {
  /* Quality Settings */
  LilybearQualityLevel quality_level; /* Base quality level */
  int base_samples;             /* Base samples per pixel */
  float quality_factor;         /* Quality adjustment factor */
  
  /* Live Rendering Settings */
  bool enable_live_rendering;   /* Enable live rendering mode */
  float live_update_rate;       /* Live update rate in Hz */
  int progressive_passes;       /* Number of progressive passes */
  
  /* AI Settings */
  bool enable_ai_integration;   /* Enable AI integration */
  bool enable_ai_suggestions;   /* Enable AI suggestions */
  bool enable_ai_optimization;  /* Enable AI optimization */
  float ai_confidence_threshold; /* Minimum AI confidence for suggestions */
  
  /* Performance Settings */
  bool enable_adaptive_quality; /* Enable adaptive quality adjustment */
  bool enable_performance_monitoring; /* Enable performance monitoring */
  float performance_target_fps; /* Target FPS for adaptive rendering */
  
  /* Advanced Settings */
  bool enable_intelligent_sampling; /* Enable intelligent sampling */
  bool enable_real_time_denoising; /* Enable real-time denoising */
  bool enable_workflow_automation; /* Enable workflow automation */
};

/* -------------------------------------------------------------------- */
/** \name Utility Functions
 * \{ */

/**
 * Convert quality level to numeric value
 * Returns quality factor between 0.0 and 1.0
 */
float lilybear_quality_level_to_factor(LilybearQualityLevel level);

/**
 * Convert numeric value to quality level
 * Returns appropriate quality level for given factor
 */
LilybearQualityLevel lilybear_factor_to_quality_level(float factor);

/**
 * Get default render settings
 * Returns recommended default settings for Lilybear engine
 */
LilybearRenderSettings get_default_lilybear_settings();

/**
 * Validate render settings
 * Returns true if settings are valid and consistent
 */
bool validate_lilybear_settings(const LilybearRenderSettings &settings);

/* -------------------------------------------------------------------- */
/** \name Logging and Debugging
 * \{ */

/**
 * Initialize Lilybear logging
 * Sets up logging system for debugging and monitoring
 */
void init_lilybear_logging();

/**
 * Set logging level for Lilybear engine
 * Controls verbosity of logging output
 */
void set_lilybear_log_level(int level);

/**
 * Log performance metrics
 * Outputs current performance data to log
 */
void log_lilybear_performance(const LilybearPerformanceMetrics &metrics);

/**
 * Log AI suggestion
 * Outputs AI suggestion details to log
 */
void log_lilybear_ai_suggestion(const LilybearAISuggestion &suggestion);
} 