#version 420

in TES_OUT
{
  vec3 normal;
} fs_in;

out vec4 oColor;

void main()
{
  oColor = vec4(fs_in.normal, 1.0);
}
