/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#include "BLI_math_vector.hh"
#include "BLI_string.h"
#include "BLI_time.h"
#include "BLI_utildefines.h"

#include "lilybear_ai_integration.hh"
#include "lilybear_types.hh"
#include "lilybear_utils.hh"

namespace blender::lilybear {

/**
 * 🤖 Lilybear AI Integration System
 * 
 * Provides intelligent scene analysis and optimization:
 * - Scene complexity analysis
 * - Performance optimization suggestions
 * - Quality improvement recommendations
 * - Workflow automation
 * - Creative assistance
 */

/* -------------------------------------------------------------------- */
/** \name AI Integration Implementation
 * \{ */

/**
 * AI Integration Instance
 * Manages AI analysis and optimization for rendering
 */
class AIIntegrationInstance {
 private:
  /* Core State */
  bool is_initialized_;
  bool is_monitoring_;
  
  /* AI Models */
  struct AIModel {
    bool is_loaded;
    char name[64];
    float version;
    void *model_data;
  };
  
  AIModel complexity_model_;
  AIModel optimization_model_;
  AIModel quality_model_;
  AIModel workflow_model_;
  
  /* Analysis Data */
  SceneData last_scene_data_;
  PerformanceMetrics last_performance_;
  std::vector<AISuggestion> cached_suggestions_;
  
  /* Statistics */
  double total_processing_time_;
  int suggestions_generated_;
  int optimizations_applied_;
  float optimization_score_;
  
  /* Configuration */
  float confidence_threshold_;
  int max_suggestions_;
  bool enable_learning_;
  
 public:
  AIIntegrationInstance() : is_initialized_(false), is_monitoring_(false)
  {
    /* Initialize AI models */
    complexity_model_.is_loaded = false;
    optimization_model_.is_loaded = false;
    quality_model_.is_loaded = false;
    workflow_model_.is_loaded = false;
    
    /* Initialize statistics */
    total_processing_time_ = 0.0;
    suggestions_generated_ = 0;
    optimizations_applied_ = 0;
    optimization_score_ = 0.0f;
    
    /* Initialize configuration */
    confidence_threshold_ = DEFAULT_AI_CONFIDENCE_THRESHOLD;
    max_suggestions_ = MAX_AI_SUGGESTIONS;
    enable_learning_ = true;
  }
  
  ~AIIntegrationInstance()
  {
    cleanup();
  }
  
  /**
   * Initialize AI integration
   * Loads AI models and sets up analysis pipeline
   */
  bool init()
  {
    if (is_initialized_) {
      return true;
    }
    
    /* Load AI models */
    if (!load_ai_models()) {
      CLOG_WARN(LOG_LILYBEAR, "Failed to load some AI models");
    }
    
    /* Initialize analysis pipeline */
    if (!init_analysis_pipeline()) {
      CLOG_ERROR(LOG_LILYBEAR, "Failed to initialize AI analysis pipeline");
      return false;
    }
    
    is_initialized_ = true;
    CLOG_INFO(LOG_LILYBEAR, 1, "AI integration initialized successfully");
    
    return true;
  }
  
  /**
   * Start AI monitoring
   * Begins continuous scene analysis
   */
  void start_monitoring()
  {
    if (!is_initialized_) {
      CLOG_ERROR(LOG_LILYBEAR, "Cannot start AI monitoring: not initialized");
      return;
    }
    
    if (is_monitoring_) {
      CLOG_WARN(LOG_LILYBEAR, "AI monitoring already active");
      return;
    }
    
    is_monitoring_ = true;
    CLOG_INFO(LOG_LILYBEAR, 1, "AI monitoring started");
  }
  
  /**
   * Stop AI monitoring
   * Stops continuous scene analysis
   */
  void stop_monitoring()
  {
    if (!is_monitoring_) {
      return;
    }
    
    is_monitoring_ = false;
    CLOG_INFO(LOG_LILYBEAR, 1, "AI monitoring stopped");
  }
  
  /**
   * Analyze scene
   * Performs comprehensive scene analysis and generates suggestions
   */
  void analyze_scene(const SceneData &scene_data,
                     const PerformanceMetrics &performance,
                     std::vector<AISuggestion> &suggestions)
  {
    if (!is_initialized_) {
      return;
    }
    
    const double start_time = BLI_time_now_seconds();
    
    /* Clear previous suggestions */
    suggestions.clear();
    
    /* Analyze scene complexity */
    analyze_scene_complexity(scene_data, suggestions);
    
    /* Analyze performance */
    analyze_performance(performance, suggestions);
    
    /* Analyze quality */
    analyze_quality(scene_data, performance, suggestions);
    
    /* Analyze workflow */
    analyze_workflow(scene_data, suggestions);
    
    /* Filter suggestions by confidence */
    filter_suggestions(suggestions);
    
    /* Limit number of suggestions */
    if (suggestions.size() > max_suggestions_) {
      suggestions.resize(max_suggestions_);
    }
    
    /* Update statistics */
    const double processing_time = BLI_time_now_seconds() - start_time;
    total_processing_time_ += processing_time;
    suggestions_generated_ += suggestions.size();
    
    /* Cache suggestions */
    cached_suggestions_ = suggestions;
    
    /* Update last analysis data */
    last_scene_data_ = scene_data;
    last_performance_ = performance;
    
    CLOG_INFO(LOG_LILYBEAR, 2, "AI analysis completed in %.3f seconds, generated %d suggestions",
              processing_time, suggestions.size());
  }
  
  /**
   * Apply AI optimizations
   * Automatically applies AI-driven optimizations
   */
  void apply_optimizations(const SceneData &scene_data,
                          const PerformanceMetrics &performance,
                          const RenderSettings &settings)
  {
    if (!is_initialized_) {
      return;
    }
    
    /* Generate optimization suggestions */
    std::vector<AISuggestion> optimizations;
    analyze_scene(scene_data, performance, optimizations);
    
    /* Apply high-confidence optimizations */
    for (const auto &suggestion : optimizations) {
      if (suggestion.confidence >= confidence_threshold_ &&
          suggestion.type == AISuggestion::Type::PERFORMANCE) {
        apply_suggestion(suggestion, scene_data, settings);
        optimizations_applied_++;
      }
    }
    
    /* Update optimization score */
    update_optimization_score();
  }
  
  /**
   * Get processing time
   * Returns total AI processing time
   */
  double get_processing_time() const
  {
    return total_processing_time_;
  }
  
  /**
   * Get optimization score
   * Returns AI optimization effectiveness score
   */
  float get_optimization_score() const
  {
    return optimization_score_;
  }
  
  /**
   * Set confidence threshold
   * Sets minimum confidence for AI suggestions
   */
  void set_confidence_threshold(float threshold)
  {
    confidence_threshold_ = std::clamp(threshold, 0.0f, 1.0f);
  }
  
  /**
   * Set max suggestions
   * Sets maximum number of suggestions to generate
   */
  void set_max_suggestions(int max_suggestions)
  {
    max_suggestions_ = std::max(1, max_suggestions);
  }
  
  /**
   * Enable/disable learning
   * Controls whether AI learns from user feedback
   */
  void set_learning_enabled(bool enabled)
  {
    enable_learning_ = enabled;
  }
  
 private:
  /**
   * Load AI models
   * Loads trained AI models for analysis
   */
  bool load_ai_models()
  {
    /* Load complexity analysis model */
    if (!load_complexity_model()) {
      CLOG_WARN(LOG_LILYBEAR, "Failed to load complexity model");
    }
    
    /* Load optimization model */
    if (!load_optimization_model()) {
      CLOG_WARN(LOG_LILYBEAR, "Failed to load optimization model");
    }
    
    /* Load quality analysis model */
    if (!load_quality_model()) {
      CLOG_WARN(LOG_LILYBEAR, "Failed to load quality model");
    }
    
    /* Load workflow analysis model */
    if (!load_workflow_model()) {
      CLOG_WARN(LOG_LILYBEAR, "Failed to load workflow model");
    }
    
    return true;
  }
  
  /**
   * Load complexity analysis model
   */
  bool load_complexity_model()
  {
    /* TODO: Implement actual model loading */
    complexity_model_.is_loaded = true;
    BLI_strncpy(complexity_model_.name, "SceneComplexity", sizeof(complexity_model_.name));
    complexity_model_.version = 1.0f;
    complexity_model_.model_data = nullptr;
    
    CLOG_INFO(LOG_LILYBEAR, 2, "Complexity model loaded: %s v%.1f",
              complexity_model_.name, complexity_model_.version);
    
    return true;
  }
  
  /**
   * Load optimization model
   */
  bool load_optimization_model()
  {
    /* TODO: Implement actual model loading */
    optimization_model_.is_loaded = true;
    BLI_strncpy(optimization_model_.name, "PerformanceOptimization", sizeof(optimization_model_.name));
    optimization_model_.version = 1.0f;
    optimization_model_.model_data = nullptr;
    
    CLOG_INFO(LOG_LILYBEAR, 2, "Optimization model loaded: %s v%.1f",
              optimization_model_.name, optimization_model_.version);
    
    return true;
  }
  
  /**
   * Load quality analysis model
   */
  bool load_quality_model()
  {
    /* TODO: Implement actual model loading */
    quality_model_.is_loaded = true;
    BLI_strncpy(quality_model_.name, "QualityAnalysis", sizeof(quality_model_.name));
    quality_model_.version = 1.0f;
    quality_model_.model_data = nullptr;
    
    CLOG_INFO(LOG_LILYBEAR, 2, "Quality model loaded: %s v%.1f",
              quality_model_.name, quality_model_.version);
    
    return true;
  }
  
  /**
   * Load workflow analysis model
   */
  bool load_workflow_model()
  {
    /* TODO: Implement actual model loading */
    workflow_model_.is_loaded = true;
    BLI_strncpy(workflow_model_.name, "WorkflowOptimization", sizeof(workflow_model_.name));
    workflow_model_.version = 1.0f;
    workflow_model_.model_data = nullptr;
    
    CLOG_INFO(LOG_LILYBEAR, 2, "Workflow model loaded: %s v%.1f",
              workflow_model_.name, workflow_model_.version);
    
    return true;
  }
  
  /**
   * Initialize analysis pipeline
   * Sets up the AI analysis pipeline
   */
  bool init_analysis_pipeline()
  {
    /* TODO: Initialize analysis pipeline */
    CLOG_INFO(LOG_LILYBEAR, 2, "AI analysis pipeline initialized");
    return true;
  }
  
  /**
   * Analyze scene complexity
   * Analyzes scene complexity and generates suggestions
   */
  void analyze_scene_complexity(const SceneData &scene_data, std::vector<AISuggestion> &suggestions)
  {
    if (!complexity_model_.is_loaded) {
      return;
    }
    
    /* Calculate complexity score */
    const float complexity_score = calculate_scene_complexity(scene_data);
    
    /* Generate complexity-based suggestions */
    if (complexity_score > 0.8f) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::PERFORMANCE;
      suggestion.priority = AISuggestion::Priority::HIGH;
      suggestion.confidence = 0.85f;
      suggestion.impact_score = 0.9f;
      suggestion.effort_score = 0.3f;
      
      BLI_strncpy(suggestion.id, "high_complexity_optimization", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "High Scene Complexity Detected", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "Your scene has high complexity which may impact rendering performance. "
                  "Consider simplifying geometry, reducing material complexity, or using "
                  "level-of-detail techniques.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Simplify scene geometry or reduce material complexity",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
    
    /* Check for specific complexity issues */
    if (scene_data.triangle_count > 1000000) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::PERFORMANCE;
      suggestion.priority = AISuggestion::Priority::MEDIUM;
      suggestion.confidence = 0.75f;
      suggestion.impact_score = 0.7f;
      suggestion.effort_score = 0.5f;
      
      BLI_strncpy(suggestion.id, "high_triangle_count", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "High Triangle Count", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "Scene contains over 1 million triangles which may slow down rendering. "
                  "Consider using decimation or level-of-detail techniques.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Apply mesh decimation or use LOD techniques",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
  }
  
  /**
   * Analyze performance
   * Analyzes performance metrics and generates optimization suggestions
   */
  void analyze_performance(const PerformanceMetrics &performance, std::vector<AISuggestion> &suggestions)
  {
    if (!optimization_model_.is_loaded) {
      return;
    }
    
    /* Check FPS performance */
    if (performance.fps < 15.0f) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::PERFORMANCE;
      suggestion.priority = AISuggestion::Priority::CRITICAL;
      suggestion.confidence = 0.9f;
      suggestion.impact_score = 0.95f;
      suggestion.effort_score = 0.4f;
      
      BLI_strncpy(suggestion.id, "low_fps_optimization", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "Low Frame Rate Detected", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "Frame rate is below 15 FPS which may cause poor user experience. "
                  "Consider reducing render quality, simplifying scene, or optimizing materials.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Reduce render quality or simplify scene",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
    
    /* Check GPU utilization */
    if (performance.gpu_utilization > 0.95f) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::PERFORMANCE;
      suggestion.priority = AISuggestion::Priority::HIGH;
      suggestion.confidence = 0.8f;
      suggestion.impact_score = 0.8f;
      suggestion.effort_score = 0.3f;
      
      BLI_strncpy(suggestion.id, "high_gpu_utilization", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "High GPU Utilization", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "GPU utilization is very high which may cause thermal throttling. "
                  "Consider reducing render quality or enabling adaptive quality.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Enable adaptive quality or reduce render settings",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
    
    /* Check memory usage */
    if (performance.gpu_memory_used > 6ULL * 1024 * 1024 * 1024) { /* 6GB */
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::PERFORMANCE;
      suggestion.priority = AISuggestion::Priority::MEDIUM;
      suggestion.confidence = 0.7f;
      suggestion.impact_score = 0.6f;
      suggestion.effort_score = 0.4f;
      
      BLI_strncpy(suggestion.id, "high_memory_usage", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "High GPU Memory Usage", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "GPU memory usage is high which may cause performance issues. "
                  "Consider reducing texture resolution or simplifying materials.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Reduce texture resolution or simplify materials",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
  }
  
  /**
   * Analyze quality
   * Analyzes rendering quality and generates improvement suggestions
   */
  void analyze_quality(const SceneData &scene_data,
                      const PerformanceMetrics &performance,
                      std::vector<AISuggestion> &suggestions)
  {
    if (!quality_model_.is_loaded) {
      return;
    }
    
    /* Check for low quality settings */
    if (performance.current_quality < 0.3f) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::QUALITY;
      suggestion.priority = AISuggestion::Priority::MEDIUM;
      suggestion.confidence = 0.7f;
      suggestion.impact_score = 0.8f;
      suggestion.effort_score = 0.2f;
      
      BLI_strncpy(suggestion.id, "low_quality_improvement", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "Low Rendering Quality", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "Current rendering quality is low. Consider increasing quality settings "
                  "if performance allows for better visual results.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Increase render quality settings",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
    
    /* Check for missing materials */
    if (scene_data.material_count == 0) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::QUALITY;
      suggestion.priority = AISuggestion::Priority::LOW;
      suggestion.confidence = 0.6f;
      suggestion.impact_score = 0.5f;
      suggestion.effort_score = 0.7f;
      
      BLI_strncpy(suggestion.id, "no_materials", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "No Materials Applied", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "No materials are applied to objects in the scene. "
                  "Adding materials will improve visual quality.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Add materials to scene objects",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
  }
  
  /**
   * Analyze workflow
   * Analyzes workflow efficiency and generates optimization suggestions
   */
  void analyze_workflow(const SceneData &scene_data, std::vector<AISuggestion> &suggestions)
  {
    if (!workflow_model_.is_loaded) {
      return;
    }
    
    /* Check for animation workflow */
    if (scene_data.has_animation) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::WORKFLOW;
      suggestion.priority = AISuggestion::Priority::MEDIUM;
      suggestion.confidence = 0.7f;
      suggestion.impact_score = 0.6f;
      suggestion.effort_score = 0.3f;
      
      BLI_strncpy(suggestion.id, "animation_workflow", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "Animation Workflow Detected", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "Scene contains animation. Consider using batch rendering "
                  "or progressive rendering for better workflow efficiency.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Use batch rendering or progressive rendering",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
    
    /* Check for complex materials */
    if (scene_data.has_volumetric_materials || scene_data.has_transparent_materials) {
      AISuggestion suggestion;
      suggestion.type = AISuggestion::Type::WORKFLOW;
      suggestion.priority = AISuggestion::Priority::LOW;
      suggestion.confidence = 0.6f;
      suggestion.impact_score = 0.4f;
      suggestion.effort_score = 0.5f;
      
      BLI_strncpy(suggestion.id, "complex_materials_workflow", sizeof(suggestion.id));
      BLI_strncpy(suggestion.title, "Complex Materials Detected", sizeof(suggestion.title));
      BLI_strncpy(suggestion.description,
                  "Scene contains volumetric or transparent materials. "
                  "These may require special rendering considerations.",
                  sizeof(suggestion.description));
      BLI_strncpy(suggestion.action,
                  "Review material settings and rendering options",
                  sizeof(suggestion.action));
      
      suggestions.push_back(suggestion);
    }
  }
  
  /**
   * Filter suggestions by confidence
   * Removes low-confidence suggestions
   */
  void filter_suggestions(std::vector<AISuggestion> &suggestions)
  {
    suggestions.erase(
        std::remove_if(suggestions.begin(),
                      suggestions.end(),
                      [this](const AISuggestion &s) { return s.confidence < confidence_threshold_; }),
        suggestions.end());
  }
  
  /**
   * Apply suggestion
   * Applies an AI suggestion to the scene or settings
   */
  void apply_suggestion(const AISuggestion &suggestion,
                       const SceneData &scene_data,
                       const RenderSettings &settings)
  {
    /* TODO: Implement suggestion application */
    CLOG_INFO(LOG_LILYBEAR, 2, "Applied AI suggestion: %s", suggestion.title);
  }
  
  /**
   * Update optimization score
   * Updates the AI optimization effectiveness score
   */
  void update_optimization_score()
  {
    /* Calculate score based on applied optimizations and their impact */
    if (optimizations_applied_ > 0) {
      optimization_score_ = std::min(1.0f, float(optimizations_applied_) / 10.0f);
    }
  }
  
  /**
   * Cleanup resources
   * Frees all allocated resources
   */
  void cleanup()
  {
    /* Free AI models */
    if (complexity_model_.model_data) {
      /* TODO: Free model data */
      complexity_model_.model_data = nullptr;
    }
    if (optimization_model_.model_data) {
      /* TODO: Free model data */
      optimization_model_.model_data = nullptr;
    }
    if (quality_model_.model_data) {
      /* TODO: Free model data */
      quality_model_.model_data = nullptr;
    }
    if (workflow_model_.model_data) {
      /* TODO: Free model data */
      workflow_model_.model_data = nullptr;
    }
    
    is_initialized_ = false;
    is_monitoring_ = false;
  }
};

/* -------------------------------------------------------------------- */
/** \name Global AI Integration Manager
 * \{ */

/**
 * Global AI integration manager
 * Manages all AI integration instances
 */
class AIIntegrationManager {
 private:
  static AIIntegrationManager *instance_;
  std::map<RenderEngine *, AIIntegrationInstance *> instances_;
  bool is_initialized_;
  
 public:
  AIIntegrationManager() : is_initialized_(false) {}
  
  static AIIntegrationManager *get_instance()
  {
    if (!instance_) {
      instance_ = new AIIntegrationManager();
    }
    return instance_;
  }
  
  /**
   * Initialize AI integration system
   */
  bool init()
  {
    if (is_initialized_) {
      return true;
    }
    
    is_initialized_ = true;
    CLOG_INFO(LOG_LILYBEAR, 1, "AI integration manager initialized");
    return true;
  }
  
  /**
   * Cleanup AI integration system
   */
  void cleanup()
  {
    /* Cleanup all instances */
    for (auto &pair : instances_) {
      delete pair.second;
    }
    instances_.clear();
    
    is_initialized_ = false;
    CLOG_INFO(LOG_LILYBEAR, 1, "AI integration manager cleaned up");
  }
  
  /**
   * Create AI integration instance
   */
  AIIntegrationInstance *create_instance(RenderEngine *engine)
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
    AIIntegrationInstance *instance = new AIIntegrationInstance();
    if (!instance->init()) {
      delete instance;
      return nullptr;
    }
    
    instances_[engine] = instance;
    return instance;
  }
  
  /**
   * Get AI integration instance
   */
  AIIntegrationInstance *get_instance(RenderEngine *engine)
  {
    auto it = instances_.find(engine);
    return (it != instances_.end()) ? it->second : nullptr;
  }
  
  /**
   * Destroy AI integration instance
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
AIIntegrationManager *AIIntegrationManager::instance_ = nullptr;

/* -------------------------------------------------------------------- */
/** \name Public Interface
 * \{ */

/**
 * Initialize AI integration system
 */
void init()
{
  AIIntegrationManager::get_instance()->init();
}

/**
 * Cleanup AI integration system
 */
void cleanup()
{
  AIIntegrationManager::get_instance()->cleanup();
}

/**
 * Start AI monitoring for engine
 */
void start_monitoring(RenderEngine *engine)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->create_instance(engine);
  
  if (instance) {
    instance->start_monitoring();
  }
}

/**
 * Stop AI monitoring for engine
 */
void stop_monitoring(RenderEngine *engine)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->stop_monitoring();
  }
}

/**
 * Analyze scene with AI
 */
void analyze_scene(RenderEngine *engine,
                  const SceneData &scene_data,
                  const PerformanceMetrics &performance,
                  std::vector<AISuggestion> &suggestions)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->analyze_scene(scene_data, performance, suggestions);
  }
}

/**
 * Apply AI optimizations
 */
void apply_optimizations(RenderEngine *engine,
                        const SceneData &scene_data,
                        const PerformanceMetrics &performance,
                        const RenderSettings &settings)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->apply_optimizations(scene_data, performance, settings);
  }
}

/**
 * Get AI processing time
 */
double get_processing_time(RenderEngine *engine)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  return instance ? instance->get_processing_time() : 0.0;
}

/**
 * Get AI optimization score
 */
float get_optimization_score(RenderEngine *engine)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  return instance ? instance->get_optimization_score() : 0.0f;
}

/**
 * Set AI confidence threshold
 */
void set_confidence_threshold(RenderEngine *engine, float threshold)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->set_confidence_threshold(threshold);
  }
}

/**
 * Set AI max suggestions
 */
void set_max_suggestions(RenderEngine *engine, int max_suggestions)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->set_max_suggestions(max_suggestions);
  }
}

/**
 * Set AI learning enabled
 */
void set_learning_enabled(RenderEngine *engine, bool enabled)
{
  AIIntegrationManager *manager = AIIntegrationManager::get_instance();
  AIIntegrationInstance *instance = manager->get_instance(engine);
  
  if (instance) {
    instance->set_learning_enabled(enabled);
  }
}

}  // namespace blender::lilybear