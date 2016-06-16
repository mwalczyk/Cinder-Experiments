#version 420

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec3 ciColor;
in vec3 offset;

out VS_OUT
{
  vec3 color;
} vs_out;

void main()
{
  vec4 positionOffset = ciPosition + vec4(offset, 0.0);
  gl_Position = ciModelViewProjection * positionOffset;

  vs_out.color = ciColor;
}
