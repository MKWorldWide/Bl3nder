/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * Shared Emotional Resonance Awareness (ERA) shader functions.
 */

#pragma BLENDER_REQUIRE(common_math_lib.glsl)
#pragma BLENDER_REQUIRE(common_view_lib.glsl)
#pragma BLENDER_REQUIRE(eevee_shared.glsl)

/* Emotional resonance texture samplers */
uniform sampler2D depth_tx;
uniform sampler2D normal_tx;
uniform sampler2D resonance_tx;
uniform sampler2D spectral_tx;
uniform sampler2D memory_tx;
uniform sampler2D biome_tx;

/* Emotional resonance parameters */
uniform float emotional_intensity;
uniform float resonance_frequency;
uniform float memory_echo_strength;
uniform float biome_interaction_level;

/* Emotional resonance color mapping */
vec3 map_emotion_to_color(float emotion)
{
  /* Map emotion to HSV color space */
  float hue = emotion * 0.8;  /* Scale to avoid full red */
  float saturation = 0.8;
  float value = 0.8;
  
  /* Convert HSV to RGB */
  vec3 rgb = vec3(0.0);
  float h = hue * 6.0;
  float c = value * saturation;
  float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
  float m = value - c;
  
  if (h < 1.0) rgb = vec3(c, x, 0.0);
  else if (h < 2.0) rgb = vec3(x, c, 0.0);
  else if (h < 3.0) rgb = vec3(0.0, c, x);
  else if (h < 4.0) rgb = vec3(0.0, x, c);
  else if (h < 5.0) rgb = vec3(x, 0.0, c);
  else rgb = vec3(c, 0.0, x);
  
  return rgb + m;
}

/* Emotional resonance pattern generation */
float generate_resonance_pattern(vec2 uv, float frequency, float time)
{
  float pattern = 0.0;
  
  /* Base pattern */
  pattern += sin(uv.x * frequency + time) * 
             cos(uv.y * frequency + time);
  
  /* Add harmonics */
  pattern += 0.5 * sin(uv.x * frequency * 2.0 + time * 1.5) * 
             cos(uv.y * frequency * 2.0 + time * 1.5);
  
  /* Add sub-harmonics */
  pattern += 0.25 * sin(uv.x * frequency * 0.5 + time * 0.75) * 
             cos(uv.y * frequency * 0.5 + time * 0.75);
  
  return pattern;
}

/* Emotional resonance blending */
vec4 blend_resonance(vec4 base, vec4 resonance, float strength)
{
  /* Additive blending with strength control */
  return base + resonance * strength;
}

/* Emotional resonance temporal persistence */
vec4 apply_temporal_persistence(vec4 current, vec4 previous, float persistence)
{
  return mix(current, previous, persistence);
}

/* Emotional resonance spatial filtering */
vec4 filter_resonance(vec2 uv, sampler2D tex, float radius)
{
  vec4 result = vec4(0.0);
  float total = 0.0;
  
  /* Simple box filter */
  for (float x = -radius; x <= radius; x += 1.0) {
    for (float y = -radius; y <= radius; y += 1.0) {
      vec2 offset = vec2(x, y) / vec2(textureSize(tex, 0));
      result += texture(tex, uv + offset);
      total += 1.0;
    }
  }
  
  return result / total;
} 