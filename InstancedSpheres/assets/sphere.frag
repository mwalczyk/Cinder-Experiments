#version 420

in vec3 vColor;
out vec4 oColor;

void main()
{
  oColor = vec4(vColor, 1.0);
}
