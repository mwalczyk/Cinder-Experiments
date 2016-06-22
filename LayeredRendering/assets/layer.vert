#version 430

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out VS_OUT
{
  vec2 texcoord;
} vs_out;

void main()
{
  vs_out.texcoord = ciTexCoord0;
  gl_Position = ciModelViewProjection * ciPosition;
}
