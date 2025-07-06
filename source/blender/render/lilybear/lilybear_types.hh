/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_sys_types.h"
#include "BLI_math_vector_types.hh"

#include "GPU_texture.hh"

namespace blender::lilybear {

/* -------------------------------------------------------------------- */
/** \name Forward Declarations
 * \{ */

class Instance;
class Pipeline;
class ShaderModule;
class AIIntegration;
class LiveRendering;

/* -------------------------------------------------------------------- */
/** \name Basic Types
 * \{ */

/**
 * 2D integer vector for texture coordinates and screen space
 */
using int2 = blender::int2;

/**
 * 2D float vector for normalized coordinates
 */
using float2 = blender::float2;

/**
 * 3D float vector for world space coordinates
 */
using float3 = blender::float3;

/**
 * 4D float vector for homogeneous coordinates
 */
using float4 = blender::float4;

/**
 * 4x4 transformation matrix
 */
using float4x4 = blender::float4x4;

/* -------------------------------------------------------------------- */
/** \name Enums and Constants
 * \{ */

/**
 * Lilybear Render Modes
 * Different rendering modes for various use cases
 */
enum class RenderMode {
  VIEWPORT,      /* Real-time viewport rendering */
  PREVIEW,       /* Quick preview rendering */
  FINAL,         /* Final quality rendering */
  BATCH,         /* Batch rendering for animations */
  INTERACTIVE,   /* Interactive rendering with live updates */
};

/**
 * Lilybear Sampling Strategies
 * Different approaches to sample distribution
 */
enum class SamplingStrategy {
  UNIFORM,       /* Uniform sampling across the image */
  ADAPTIVE,      /* Adaptive sampling based on variance */
  INTELLIGENT,   /* AI-driven intelligent sampling */
  PROGRESSIVE,   /* Progressive refinement sampling */
  IMPORTANCE,    /* Importance-based sampling */
};

/**
 * Lilybear Denoising Methods
 * Different denoising approaches for noise reduction
 */
enum class DenoisingMethod {
  NONE,          /* No denoising */
  GAUSSIAN,      /* Simple Gaussian blur */
  BILATERAL,     /* Bilateral filtering */
  AI_DENOISER,   /* AI-powered denoising */
  REAL_TIME,     /* Real-time denoising for viewport */
};

/**
 * Lilybear Quality Presets
 * Predefined quality levels for different use cases
 */
enum class QualityPreset {
  DRAFT,         /* Fastest, lowest quality */
  PREVIEW,       /* Quick preview quality */
  GOOD,          /* Good quality for most work */
  HIGH,          /* High quality for final work */
  PRODUCTION,    /* Production quality */
  CUSTOM,        /* Custom quality settings */
};

/* -------------------------------------------------------------------- */
/** \name Data Structures
 * \{ */

/**
 * Lilybear Render Statistics
 * Comprehensive statistics for monitoring and optimization
 */
struct RenderStats {
  /* Timing Information */
  double total_time;            /* Total render time in seconds */
  double setup_time;            /* Setup time in seconds */
  double render_time;           /* Pure rendering time in seconds */
  double post_process_time;     /* Post-processing time in seconds */
  
  /* Performance Metrics */
  float fps;                    /* Frames per second */
  float frame_time_ms;          /* Average frame time in milliseconds */
  float gpu_utilization;        /* GPU utilization percentage */
  float cpu_utilization;        /* CPU utilization percentage */
  
  /* Memory Usage */
  size_t gpu_memory_used;       /* GPU memory used in bytes */
  size_t system_memory_used;    /* System memory used in bytes */
  size_t texture_memory_used;   /* Texture memory used in bytes */
  
  /* Quality Metrics */
  int samples_per_pixel;        /* Samples per pixel */
  float noise_level;            /* Estimated noise level */
  float convergence_ratio;      /* Convergence ratio (0.0 - 1.0) */
  
  /* AI Metrics */
  double ai_processing_time;    /* AI processing time in seconds */
  int ai_suggestions_generated; /* Number of AI suggestions generated */
  float ai_optimization_score;  /* AI optimization effectiveness */
  
  /* Live Rendering Metrics */
  int live_updates;             /* Number of live updates */
  float live_update_frequency;  /* Live update frequency in Hz */
  bool live_rendering_active;   /* Whether live rendering is active */
};

/**
 * Lilybear Render Settings
 * Comprehensive settings for the render engine
 */
struct RenderSettings {
  /* Basic Settings */
  int2 resolution;              /* Render resolution */
  float aspect_ratio;           /* Aspect ratio */
  bool use_crop;                /* Use crop window */
  float2 crop_min, crop_max;    /* Crop window coordinates */
  
  /* Quality Settings */
  QualityPreset quality_preset; /* Quality preset */
  int samples_per_pixel;        /* Samples per pixel */
  float quality_factor;         /* Quality adjustment factor (0.0 - 1.0) */
  int max_bounces;              /* Maximum light bounces */
  
  /* Sampling Settings */
  SamplingStrategy sampling_strategy; /* Sampling strategy */
  float sampling_threshold;     /* Sampling threshold for adaptive sampling */
  int min_samples;              /* Minimum samples per pixel */
  int max_samples;              /* Maximum samples per pixel */
  
  /* Denoising Settings */
  DenoisingMethod denoising_method; /* Denoising method */
  float denoising_strength;     /* Denoising strength (0.0 - 1.0) */
  bool enable_denoising;        /* Enable denoising */
  
  /* Live Rendering Settings */
  bool enable_live_rendering;   /* Enable live rendering */
  float live_update_rate;       /* Live update rate in Hz */
  int progressive_passes;       /* Number of progressive passes */
  bool enable_interactive_mode; /* Enable interactive mode */
  
  /* AI Settings */
  bool enable_ai_integration;   /* Enable AI integration */
  bool enable_ai_suggestions;   /* Enable AI suggestions */
  bool enable_ai_optimization;  /* Enable AI optimization */
  float ai_confidence_threshold; /* AI confidence threshold */
  
  /* Performance Settings */
  bool enable_adaptive_quality; /* Enable adaptive quality */
  bool enable_performance_monitoring; /* Enable performance monitoring */
  float target_fps;             /* Target FPS for adaptive rendering */
  float max_render_time;        /* Maximum render time in seconds */
  
  /* Advanced Settings */
  bool enable_intelligent_sampling; /* Enable intelligent sampling */
  bool enable_real_time_denoising; /* Enable real-time denoising */
  bool enable_workflow_automation; /* Enable workflow automation */
  bool enable_debug_output;     /* Enable debug output */
};

/**
 * Lilybear Scene Data
 * Scene information for rendering and AI analysis
 */
struct SceneData {
  /* Scene Information */
  int object_count;             /* Number of objects in scene */
  int light_count;              /* Number of lights in scene */
  int material_count;           /* Number of materials in scene */
  int texture_count;            /* Number of textures in scene */
  
  /* Geometry Information */
  int triangle_count;           /* Total triangle count */
  int vertex_count;             /* Total vertex count */
  float scene_bounds[6];        /* Scene bounding box */
  
  /* Lighting Information */
  float total_light_power;      /* Total light power in scene */
  int light_types[8];           /* Count of different light types */
  bool has_environment_light;   /* Whether scene has environment lighting */
  
  /* Material Information */
  int material_types[16];       /* Count of different material types */
  bool has_volumetric_materials; /* Whether scene has volumetric materials */
  bool has_transparent_materials; /* Whether scene has transparent materials */
  
  /* Camera Information */
  float3 camera_position;       /* Camera position */
  float3 camera_direction;      /* Camera direction */
  float camera_fov;             /* Camera field of view */
  float camera_near, camera_far; /* Camera near and far planes */
  
  /* Animation Information */
  bool has_animation;           /* Whether scene has animation */
  int frame_count;              /* Number of animation frames */
  float animation_duration;     /* Animation duration in seconds */
};

/**
 * Lilybear AI Suggestion
 * Individual AI-generated suggestion
 */
struct AISuggestion {
  /* Basic Information */
  char id[64];                  /* Unique suggestion ID */
  char title[256];              /* Suggestion title */
  char description[1024];       /* Detailed description */
  char action[512];             /* Recommended action */
  
  /* Classification */
  enum class Type {
    PERFORMANCE,                /* Performance optimization */
    QUALITY,                    /* Quality improvement */
    WORKFLOW,                   /* Workflow optimization */
    CREATIVE,                   /* Creative suggestions */
    TECHNICAL,                  /* Technical improvements */
  } type;
  
  enum class Priority {
    LOW,                        /* Low priority */
    MEDIUM,                     /* Medium priority */
    HIGH,                       /* High priority */
    CRITICAL,                   /* Critical priority */
  } priority;
  
  /* Metrics */
  float confidence;             /* AI confidence level (0.0 - 1.0) */
  float impact_score;           /* Expected impact score (0.0 - 1.0) */
  float effort_score;           /* Required effort score (0.0 - 1.0) */
  
  /* State */
  bool applied;                 /* Whether suggestion has been applied */
  bool dismissed;               /* Whether suggestion has been dismissed */
  double timestamp;             /* When suggestion was generated */
  
  /* Parameters */
  float parameters[16];         /* Suggestion parameters */
  char parameter_names[16][32]; /* Parameter names */
};

/**
 * Lilybear Performance Metrics
 * Real-time performance data
 */
struct PerformanceMetrics {
  /* Timing */
  double frame_time;            /* Frame time in seconds */
  double render_time;           /* Render time in seconds */
  double gpu_time;              /* GPU time in seconds */
  double cpu_time;              /* CPU time in seconds */
  
  /* Throughput */
  float fps;                    /* Frames per second */
  float samples_per_second;     /* Samples per second */
  float pixels_per_second;      /* Pixels per second */
  
  /* Utilization */
  float gpu_utilization;        /* GPU utilization percentage */
  float cpu_utilization;        /* CPU utilization percentage */
  float memory_utilization;     /* Memory utilization percentage */
  
  /* Memory */
  size_t gpu_memory_used;       /* GPU memory used in bytes */
  size_t system_memory_used;    /* System memory used in bytes */
  size_t texture_memory_used;   /* Texture memory used in bytes */
  
  /* Quality */
  float current_quality;        /* Current quality level */
  float target_quality;         /* Target quality level */
  float convergence_ratio;      /* Convergence ratio */
  
  /* System */
  float temperature;            /* GPU temperature in Celsius */
  float power_consumption;      /* Power consumption in watts */
  int fan_speed;                /* Fan speed in RPM */
};

/**
 * Lilybear Texture Data
 * Texture information and management
 */
struct TextureData {
  /* Basic Information */
  char name[256];               /* Texture name */
  int2 size;                    /* Texture size */
  int channels;                 /* Number of channels */
  int mipmap_levels;            /* Number of mipmap levels */
  
  /* Format Information */
  enum class Format {
    RGBA8,                      /* 8-bit RGBA */
    RGBA16,                     /* 16-bit RGBA */
    RGBA32F,                    /* 32-bit float RGBA */
    RGB8,                       /* 8-bit RGB */
    RGB16,                      /* 16-bit RGB */
    RGB32F,                     /* 32-bit float RGB */
    R8,                         /* 8-bit red */
    R16,                        /* 16-bit red */
    R32F,                       /* 32-bit float red */
  } format;
  
  /* Usage Information */
  bool is_loaded;               /* Whether texture is loaded */
  bool is_compressed;           /* Whether texture is compressed */
  size_t memory_size;           /* Memory size in bytes */
  
  /* GPU Information */
  GPUTexture *gpu_texture;      /* GPU texture handle */
  bool gpu_uploaded;            /* Whether uploaded to GPU */
};

/**
 * Lilybear Material Data
 * Material information and properties
 */
struct MaterialData {
  /* Basic Information */
  char name[256];               /* Material name */
  char type[64];                /* Material type */
  
  /* Properties */
  float3 base_color;            /* Base color */
  float metallic;               /* Metallic factor */
  float roughness;              /* Roughness factor */
  float ior;                    /* Index of refraction */
  float transmission;           /* Transmission factor */
  float alpha;                  /* Alpha factor */
  
  /* Textures */
  TextureData *base_color_texture;     /* Base color texture */
  TextureData *metallic_texture;       /* Metallic texture */
  TextureData *roughness_texture;      /* Roughness texture */
  TextureData *normal_texture;         /* Normal texture */
  TextureData *emission_texture;       /* Emission texture */
  
  /* Advanced Properties */
  bool is_volumetric;           /* Whether material is volumetric */
  bool is_transparent;          /* Whether material is transparent */
  bool is_emissive;             /* Whether material is emissive */
  
  /* AI Properties */
  float ai_complexity_score;    /* AI complexity score */
  float ai_quality_score;       /* AI quality score */
  bool ai_optimized;            /* Whether AI has optimized this material */
};

/* -------------------------------------------------------------------- */
/** \name Utility Functions
 * \{ */

/**
 * Convert quality preset to numeric factor
 * Returns quality factor between 0.0 and 1.0
 */
float quality_preset_to_factor(QualityPreset preset);

/**
 * Convert numeric factor to quality preset
 * Returns appropriate quality preset for given factor
 */
QualityPreset factor_to_quality_preset(float factor);

/**
 * Get default render settings
 * Returns recommended default settings
 */
RenderSettings get_default_render_settings();

/**
 * Validate render settings
 * Returns true if settings are valid
 */
bool validate_render_settings(const RenderSettings &settings);

/**
 * Calculate scene complexity score
 * Returns complexity score based on scene data
 */
float calculate_scene_complexity(const SceneData &scene_data);

/**
 * Estimate render time
 * Estimates render time based on settings and scene data
 */
double estimate_render_time(const RenderSettings &settings, const SceneData &scene_data);

/* -------------------------------------------------------------------- */
/** \name Constants
 * \{ */

/* Default Values */
constexpr float DEFAULT_QUALITY_FACTOR = 0.5f;
constexpr int DEFAULT_SAMPLES_PER_PIXEL = 128;
constexpr float DEFAULT_TARGET_FPS = 30.0f;
constexpr float DEFAULT_LIVE_UPDATE_RATE = 10.0f;

/* Limits */
constexpr int MAX_SAMPLES_PER_PIXEL = 16384;
constexpr int MIN_SAMPLES_PER_PIXEL = 1;
constexpr float MAX_QUALITY_FACTOR = 1.0f;
constexpr float MIN_QUALITY_FACTOR = 0.1f;

/* Performance Targets */
constexpr float TARGET_FRAME_TIME_MS = 33.33f; /* 30 FPS */
constexpr float MAX_GPU_UTILIZATION = 0.9f;
constexpr float MAX_MEMORY_UTILIZATION = 0.8f;

/* AI Settings */
constexpr float DEFAULT_AI_CONFIDENCE_THRESHOLD = 0.7f;
constexpr int MAX_AI_SUGGESTIONS = 10;

}  // namespace blender::lilybear 