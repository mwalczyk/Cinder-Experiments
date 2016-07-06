#version 430

uniform mat4 ciModelViewProjection;
in vec4 ciPosition;
in vec4 ciInstancePosition;
in vec3 ciColor;
in vec3 ciNormal;

out VS_OUT
{
  vec3 color;
} vs_out;

void main()
{
  vs_out.color = ciColor;
  gl_Position = ciModelViewProjection * vec4(ciPosition.xyz + ciInstancePosition.xyz, 1.0);
}
