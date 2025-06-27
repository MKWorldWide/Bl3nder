/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup eevee
 *
 * Emotional Resonance Awareness (ERA) shader definitions.
 */

#pragma once

#include "eevee_shader_shared.hh"

namespace blender::eevee {

/* Emotional resonance shader types */
enum ERAShaderType {
  ERA_RESONANCE = 0,
  ERA_SPECTRAL = 1,
  ERA_MEMORY = 2,
  ERA_BIOME = 3,
};

/* Emotional resonance shader parameters */
struct ERAUniformData {
  float emotional_intensity;
  float resonance_frequency;
  float memory_echo_strength;
  float biome_interaction_level;
};

/* Emotional resonance shader outputs */
struct ERAOutput {
  float4 resonance;
  float4 spectral;
  float4 memory;
  float4 biome;
};

/* Emotional resonance shader inputs */
struct ERAInput {
  float4 depth;
  float4 normal;
  float4 color;
  float4 velocity;
};

}  // namespace blender::eevee 