#version 420

in VS_OUT
{
  vec3 color;
} fs_in;

out vec4 oColor;

void main()
{
  oColor = vec4(fs_in.color, 1.0);
}
