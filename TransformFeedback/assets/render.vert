#version 420

uniform mat4 ciModelViewProjection;

layout (location = 0) in vec4 position;

void main()
{
  gl_Position = ciModelViewProjection * position;
}
