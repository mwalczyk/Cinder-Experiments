#version 420

layout (vertices = 4) out;

uniform float uTessLevelInner = 100.0;
uniform float uTessLevelOuter = 15.0;

in VS_OUT
{
  vec2 texcoord;
} tcs_in[];

out TCS_OUT
{
  vec2 texcoord;
} tcs_out[];

patch out vec3 tcsPatchColor;

void main()
{
  if (gl_InvocationID == 0)
  {
    gl_TessLevelInner[0] = uTessLevelInner;
    gl_TessLevelInner[1] = uTessLevelInner;
    gl_TessLevelOuter[0] = uTessLevelOuter;
    gl_TessLevelOuter[1] = uTessLevelOuter;
    gl_TessLevelOuter[2] = uTessLevelOuter;
    gl_TessLevelOuter[3] = uTessLevelOuter;
  }

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  tcs_out[gl_InvocationID].texcoord = tcs_in[gl_InvocationID].texcoord;

  tcsPatchColor = vec3(1.0, 0.0, 0.0);
}
