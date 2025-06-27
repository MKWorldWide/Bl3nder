/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * Emotional Resonance Shader
 * Visualizes emotional resonance patterns in the scene.
 */

#pragma BLENDER_REQUIRE(common_math_lib.glsl)
#pragma BLENDER_REQUIRE(common_view_lib.glsl)
#pragma BLENDER_REQUIRE(eevee_shared.glsl)
#pragma BLENDER_REQUIRE(eevee_era_shared.glsl)

uniform float emotional_intensity;
uniform float resonance_frequency;

layout(location = 0) out vec4 resonance_out;

void main()
{
  /* Get screen coordinates */
  vec2 uv = gl_FragCoord.xy / vec2(textureSize(depth_tx, 0));
  
  /* Sample depth and normal */
  float depth = texture(depth_tx, uv).r;
  vec3 normal = texture(normal_tx, uv).rgb;
  
  /* Calculate emotional resonance pattern */
  float time = float(drw_frame) * 0.1;
  float pattern = sin(uv.x * resonance_frequency + time) * 
                 cos(uv.y * resonance_frequency + time);
  
  /* Apply emotional intensity */
  pattern *= emotional_intensity;
  
  /* Modulate with normal and depth */
  float depth_factor = 1.0 - depth;
  float normal_factor = dot(normal, vec3(0.0, 0.0, 1.0));
  
  /* Combine factors */
  float resonance = pattern * depth_factor * normal_factor;
  
  /* Output resonance color */
  resonance_out = vec4(resonance, resonance * 0.5, resonance * 0.25, 1.0);
} 