#version 430

in VS_OUT
{
  vec3 color;
} fs_in;

out vec4 oColor;

void main()
{
  vec3 color = mix(fs_in.color, vec3(0.0, 0.2, 0.9), 0.5);
  oColor = vec4(color, 1.0);
}
