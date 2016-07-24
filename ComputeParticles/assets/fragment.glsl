#version 430

out vec4 oColor;

in VS_OUT
{
  vec3 color;
} fs_in;

void main()
{
  vec2 uv = gl_PointCoord * 2.0 - 1.0;
  const float radius = 0.1;
  float distFromCenter = length(uv);
  oColor = vec4(fs_in.color, 1.0 - smoothstep(radius, radius + 0.5, distFromCenter));
}
