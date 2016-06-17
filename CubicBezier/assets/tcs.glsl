#version 420

layout (vertices = 16) out;

void main()
{
  if (gl_InvocationID == 0)
  {
    gl_TessLevelInner[0] = 16.0;
    gl_TessLevelInner[1] = 16.0;
    gl_TessLevelOuter[0] = 16.0;
    gl_TessLevelOuter[1] = 16.0;
    gl_TessLevelOuter[2] = 16.0;
    gl_TessLevelOuter[3] = 16.0;
  }
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
