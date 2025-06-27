/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * Biome Interaction Shader
 * Handles emotional interactions with the biome environment.
 */

#pragma BLENDER_REQUIRE(common_math_lib.glsl)
#pragma BLENDER_REQUIRE(common_view_lib.glsl)
#pragma BLENDER_REQUIRE(eevee_shared.glsl)
#pragma BLENDER_REQUIRE(eevee_era_shared.glsl)

uniform float biome_interaction_level;

layout(location = 0) out vec4 biome_out;

void main()
{
  /* Get screen coordinates */
  vec2 uv = gl_FragCoord.xy / vec2(textureSize(memory_tx, 0));
  
  /* Sample memory texture */
  vec4 memory = texture(memory_tx, uv);
  
  /* Calculate biome interaction pattern */
  float time = float(drw_frame) * 0.03;
  vec2 biome_uv = uv + vec2(
    sin(time * memory.r) * 0.02,
    cos(time * memory.g) * 0.02
  );
  
  /* Sample biome from memory texture */
  vec4 biome = texture(memory_tx, biome_uv);
  
  /* Calculate interaction strength */
  float interaction = sin(time + memory.b * 5.0) * 0.5 + 0.5;
  interaction *= biome_interaction_level;
  
  /* Blend with memory */
  vec4 result = mix(memory, biome, interaction);
  
  /* Add organic movement */
  float organic = sin(time * 2.0 + uv.x * 10.0) * 
                 cos(time * 2.0 + uv.y * 10.0) * 0.1;
  result.rgb += vec3(organic) * biome_interaction_level;
  
  /* Output biome color */
  biome_out = result;
} 