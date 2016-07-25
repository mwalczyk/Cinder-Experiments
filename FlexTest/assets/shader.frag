#version 430

in VS_OUT
{
  vec3 color;
} fs_in;

out vec4 oColor;

void main()
{
  const vec3 water = vec3(0.1, 0.5, 0.58);
  oColor = vec4(fs_in.color + water, 1.0);
}
