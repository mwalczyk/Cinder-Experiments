#version 430

layout (binding = 0) uniform sampler2D uSceneTex;
layout (binding = 1) uniform sampler2D uBlurTex;

uniform float uExposure = 0.9;
uniform float uSceneFactor = 1.0;
uniform float uBloomFactor = 1.0;

out vec4 oColor;

in VS_OUT
{
  vec2 texcoord;
} fs_in;

void main()
{
  vec4 c = vec4(0.0);
  c += texelFetch(uSceneTex, ivec2(gl_FragCoord.xy), 0) * uSceneFactor;
  c += texelFetch(uBlurTex, ivec2(gl_FragCoord.xy), 0) * uBloomFactor;
  c.rgb = vec3(1.0) - exp(-c.rgb * uExposure);
  oColor = c;
}
