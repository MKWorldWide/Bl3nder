/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup eevee
 *
 * Emotional Resonance Awareness (ERA) system implementation.
 */

#include "GPU_debug.hh"
#include "GPU_framebuffer.hh"

#include "eevee_instance.hh"
#include "eevee_era.hh"

namespace blender::eevee {

void ERAModule::init()
{
  /* Initialize emotional resonance state */
  state_.emotional_intensity = 0.0f;
  state_.resonance_frequency = 1.0f;
  state_.memory_echo_strength = 0.0f;
  state_.biome_interaction_level = 0.0f;

  /* Initialize textures */
  const int2 extent = inst_.film.display_extent_get();
  constexpr eGPUTextureUsage usage = GPU_TEXTURE_USAGE_SHADER_READ | GPU_TEXTURE_USAGE_SHADER_WRITE;

  resonance_tx_.ensure_2d(GPU_RGBA16F, extent, usage);
  spectral_tx_.ensure_2d(GPU_RGBA16F, extent, usage);
  memory_tx_.ensure_2d(GPU_RGBA16F, extent, usage);
  biome_tx_.ensure_2d(GPU_RGBA16F, extent, usage);
}

void ERAModule::sync()
{
  /* Initialize shader passes */
  resonance_ps_.init();
  resonance_ps_.state_set(DRW_STATE_WRITE_COLOR);
  resonance_ps_.shader_set(inst_.shaders.static_shader_get(ERA_RESONANCE));
  resonance_ps_.push_constant("emotional_intensity", state_.emotional_intensity);
  resonance_ps_.push_constant("resonance_frequency", state_.resonance_frequency);
  resonance_ps_.bind_texture("depth_tx", &inst_.render_buffers.depth_tx);
  resonance_ps_.bind_texture("normal_tx", &inst_.gbuffer.normal_tx);
  resonance_ps_.bind_image("resonance_img", &resonance_tx_);

  spectral_ps_.init();
  spectral_ps_.state_set(DRW_STATE_WRITE_COLOR);
  spectral_ps_.shader_set(inst_.shaders.static_shader_get(ERA_SPECTRAL));
  spectral_ps_.push_constant("emotional_intensity", state_.emotional_intensity);
  spectral_ps_.bind_texture("resonance_tx", &resonance_tx_);
  spectral_ps_.bind_image("spectral_img", &spectral_tx_);

  memory_ps_.init();
  memory_ps_.state_set(DRW_STATE_WRITE_COLOR);
  memory_ps_.shader_set(inst_.shaders.static_shader_get(ERA_MEMORY));
  memory_ps_.push_constant("memory_echo_strength", state_.memory_echo_strength);
  memory_ps_.bind_texture("spectral_tx", &spectral_tx_);
  memory_ps_.bind_image("memory_img", &memory_tx_);

  biome_ps_.init();
  biome_ps_.state_set(DRW_STATE_WRITE_COLOR);
  biome_ps_.shader_set(inst_.shaders.static_shader_get(ERA_BIOME));
  biome_ps_.push_constant("biome_interaction_level", state_.biome_interaction_level);
  biome_ps_.bind_texture("memory_tx", &memory_tx_);
  biome_ps_.bind_image("biome_img", &biome_tx_);
}

void ERAModule::render(View &view)
{
  GPU_debug_group_begin("ERA");

  /* Render emotional resonance layers */
  {
    GPU_framebuffer_bind(inst_.render_buffers.combined_fb);
    inst_.manager->submit(resonance_ps_, view);
  }

  /* Apply spectral lighting effects */
  {
    GPU_framebuffer_bind(inst_.render_buffers.combined_fb);
    inst_.manager->submit(spectral_ps_, view);
  }

  /* Process memory-emotion echoes */
  {
    GPU_framebuffer_bind(inst_.render_buffers.combined_fb);
    inst_.manager->submit(memory_ps_, view);
  }

  /* Handle biome interactions */
  {
    GPU_framebuffer_bind(inst_.render_buffers.combined_fb);
    inst_.manager->submit(biome_ps_, view);
  }

  GPU_debug_group_end();
}

void ERAModule::set_emotional_intensity(float intensity)
{
  state_.emotional_intensity = intensity;
}

void ERAModule::set_resonance_frequency(float frequency)
{
  state_.resonance_frequency = frequency;
}

void ERAModule::set_memory_echo_strength(float strength)
{
  state_.memory_echo_strength = strength;
}

void ERAModule::set_biome_interaction_level(float level)
{
  state_.biome_interaction_level = level;
}

}  // namespace blender::eevee 