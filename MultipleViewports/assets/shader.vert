#version 430

in vec4 ciPosition;
in vec3 ciColor;

out VS_OUT
{
  vec3 color;
} vs_out;

void main()
{
  vs_out.color = ciColor;
  gl_Position = ciPosition;
}
