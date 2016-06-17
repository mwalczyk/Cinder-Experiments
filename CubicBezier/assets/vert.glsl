#version 420

uniform mat4 ciModelView;

in vec4 ciPosition;

void main()
{
  gl_Position = ciModelView * ciPosition;
}
