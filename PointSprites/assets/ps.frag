#version 430

layout (binding = 0) uniform sampler2D uSpriteTex;

out vec4 oColor;

void main()
{
  vec2 scaledPos = gl_PointCoord * 2.0 - 1.0;
  oColor = texture(uSpriteTex, gl_PointCoord);
  oColor.a = pow(1.0 - length(scaledPos), 3.0);
}
