/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include "BLI_math_vector.hh"
#include "BLI_string.h"
#include "BLI_time.h"
#include "BLI_utildefines.h"

#include "lilybear_types.hh"

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

namespace ai_integration {

/**
 * Initialize AI integration system
 * Loads AI models and sets up analysis pipeline
 */
void init();

/**
 * Cleanup AI integration system
 * Frees AI models and cleans up resources
 */
void cleanup();

/**
 * Process AI suggestions for scene optimization
 * Analyzes current scene and provides intelligent recommendations
 */
void process_suggestions(RenderEngine *engine, Depsgraph *depsgraph);

/**
 * Apply AI optimizations to render settings
 * Automatically adjusts settings for optimal quality/performance balance
 */
void apply_optimizations(RenderEngine *engine, Scene *scene);

}  // namespace ai_integration

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
  AIIntegrationInstance();
  ~AIIntegrationInstance();
  
  /**
   * Initialize AI integration
   * Loads AI models and sets up analysis pipeline
   */
  bool init();
  
  /**
   * Start AI monitoring
   * Begins continuous scene analysis
   */
  void start_monitoring();
  
  /**
   * Stop AI monitoring
   * Stops continuous scene analysis
   */
  void stop_monitoring();
  
  /**
   * Analyze scene
   * Performs comprehensive scene analysis and generates suggestions
   */
  void analyze_scene(const SceneData &scene_data,
                     const PerformanceMetrics &performance,
                     std::vector<AISuggestion> &suggestions);
  
  /**
   * Apply AI optimizations
   * Applies AI-driven optimizations to render settings
   */
  void apply_optimizations(Scene *scene, RenderSettings &settings);
  
  /**
   * Get AI suggestions
   * Returns current AI suggestions
   */
  const std::vector<AISuggestion> &get_suggestions() const;
  
  /**
   * Apply suggestion
   * Applies a specific AI suggestion
   */
  bool apply_suggestion(const AISuggestion &suggestion, Scene *scene);
  
  /**
   * Dismiss suggestion
   * Dismisses a specific AI suggestion
   */
  void dismiss_suggestion(const AISuggestion &suggestion);
  
  /**
   * Get optimization score
   * Returns current AI optimization effectiveness score
   */
  float get_optimization_score() const;
  
  /**
   * Get processing statistics
   * Returns AI processing statistics
   */
  void get_statistics(double &total_time, int &suggestions, int &optimizations) const;
  
 private:
  /**
   * Load AI models
   * Loads all required AI models
   */
  bool load_ai_models();
  
  /**
   * Initialize analysis pipeline
   * Sets up the AI analysis pipeline
   */
  bool init_analysis_pipeline();
  
  /**
   * Analyze scene complexity
   * Analyzes scene complexity and generates suggestions
   */
  void analyze_scene_complexity(const SceneData &scene_data,
                               std::vector<AISuggestion> &suggestions);
  
  /**
   * Analyze performance
   * Analyzes performance and generates optimization suggestions
   */
  void analyze_performance(const PerformanceMetrics &performance,
                          std::vector<AISuggestion> &suggestions);
  
  /**
   * Analyze quality
   * Analyzes quality and generates improvement suggestions
   */
  void analyze_quality(const SceneData &scene_data,
                      const PerformanceMetrics &performance,
                      std::vector<AISuggestion> &suggestions);
  
  /**
   * Analyze workflow
   * Analyzes workflow and generates automation suggestions
   */
  void analyze_workflow(const SceneData &scene_data,
                       std::vector<AISuggestion> &suggestions);
  
  /**
   * Filter suggestions
   * Filters suggestions by confidence and relevance
   */
  void filter_suggestions(std::vector<AISuggestion> &suggestions);
  
  /**
   * Cleanup resources
   * Frees AI models and cleans up state
   */
  void cleanup();
};

}  // namespace blender::lilybear 