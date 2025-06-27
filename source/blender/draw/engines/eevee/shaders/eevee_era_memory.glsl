/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * Memory-Emotion Echo Shader
 * Maps emotional memories to visual echoes in the scene.
 */

#pragma BLENDER_REQUIRE(common_math_lib.glsl)
#pragma BLENDER_REQUIRE(common_view_lib.glsl)
#pragma BLENDER_REQUIRE(eevee_shared.glsl)
#pragma BLENDER_REQUIRE(eevee_era_shared.glsl)

uniform float memory_echo_strength;

layout(location = 0) out vec4 memory_out;

void main()
{
  /* Get screen coordinates */
  vec2 uv = gl_FragCoord.xy / vec2(textureSize(spectral_tx, 0));
  
  /* Sample spectral texture */
  vec4 spectral = texture(spectral_tx, uv);
  
  /* Calculate memory echo pattern */
  float time = float(drw_frame) * 0.02;
  vec2 echo_uv = uv + vec2(
    sin(time + spectral.r * 10.0) * 0.01,
    cos(time + spectral.g * 10.0) * 0.01
  );
  
  /* Sample echo from spectral texture */
  vec4 echo = texture(spectral_tx, echo_uv);
  
  /* Blend with original spectral */
  vec4 memory = mix(spectral, echo, memory_echo_strength);
  
  /* Add temporal persistence */
  float persistence = 0.95;
  memory = mix(memory, texture(memory_tx, uv), persistence);
  
  /* Output memory color */
  memory_out = memory;
} 