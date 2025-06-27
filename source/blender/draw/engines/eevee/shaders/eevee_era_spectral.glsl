/* SPDX-FileCopyrightText: 2024 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * Spectral Lighting Shader
 * Applies emotional spectral effects to the scene lighting.
 */

#pragma BLENDER_REQUIRE(common_math_lib.glsl)
#pragma BLENDER_REQUIRE(common_view_lib.glsl)
#pragma BLENDER_REQUIRE(eevee_shared.glsl)
#pragma BLENDER_REQUIRE(eevee_era_shared.glsl)

uniform float emotional_intensity;

layout(location = 0) out vec4 spectral_out;

void main()
{
  /* Get screen coordinates */
  vec2 uv = gl_FragCoord.xy / vec2(textureSize(resonance_tx, 0));
  
  /* Sample resonance texture */
  vec4 resonance = texture(resonance_tx, uv);
  
  /* Calculate spectral frequencies */
  float time = float(drw_frame) * 0.05;
  vec3 spectral = vec3(
    sin(time + resonance.r * 2.0 * M_PI),
    sin(time + resonance.g * 2.0 * M_PI + M_PI/3.0),
    sin(time + resonance.b * 2.0 * M_PI + 2.0 * M_PI/3.0)
  );
  
  /* Apply emotional intensity */
  spectral *= emotional_intensity;
  
  /* Combine with resonance */
  spectral = mix(resonance.rgb, spectral, 0.5);
  
  /* Output spectral color */
  spectral_out = vec4(spectral, 1.0);
} 