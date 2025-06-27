/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup eevee
 *
 * Emotional Resonance Awareness (ERA) system for Eevee.
 * Provides real-time emotional resonance visualization and interaction.
 */

#pragma once

#include "DRW_render.hh"
#include "eevee_shader_shared.hh"
#include "eevee_sync.hh"

namespace blender::eevee {

class Instance;

/* -------------------------------------------------------------------- */
/** \name ERA Module
 * 
 * Handles emotional resonance visualization and interaction.
 * Integrates with Eevee's rendering pipeline to provide:
 * - Emotional resonance layers
 * - Spectral lighting effects
 * - Memory-emotion echo mapping
 * - Live biome interactivity
 * \{ */

class ERAModule {
 private:
  Instance &inst_;

  /* Emotional resonance data */
  struct ERAState {
    float emotional_intensity;
    float resonance_frequency;
    float memory_echo_strength;
    float biome_interaction_level;
  } state_;

  /* Shader passes */
  PassSimple resonance_ps_ = {"ERA.Resonance"};
  PassSimple spectral_ps_ = {"ERA.Spectral"};
  PassSimple memory_ps_ = {"ERA.Memory"};
  PassSimple biome_ps_ = {"ERA.Biome"};

  /* Textures */
  Texture resonance_tx_;
  Texture spectral_tx_;
  Texture memory_tx_;
  Texture biome_tx_;

 public:
  ERAModule(Instance &inst) : inst_(inst){};

  void init();
  void sync();
  void render(View &view);

  /* Emotional resonance control */
  void set_emotional_intensity(float intensity);
  void set_resonance_frequency(float frequency);
  void set_memory_echo_strength(float strength);
  void set_biome_interaction_level(float level);

  /* Texture access */
  GPUTexture *get_resonance_texture() const { return resonance_tx_; }
  GPUTexture *get_spectral_texture() const { return spectral_tx_; }
  GPUTexture *get_memory_texture() const { return memory_tx_; }
  GPUTexture *get_biome_texture() const { return biome_tx_; }
};

/** \} */

}  // namespace blender::eevee 