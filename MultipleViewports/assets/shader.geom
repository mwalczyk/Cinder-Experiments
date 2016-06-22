#version 430

// render the scene 4 times - one for each viewport
layout (triangles, invocations = 4) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 ciModelViewProjection;

layout (std140, binding = 0) uniform transformBlock
{
    mat4 rotationMatrix[4];
} block;

// inputs from the vertex shader are received as arrays
in VS_OUT
{
  vec3 color;
} gs_in[];

// outputs to the fragment shader are not
out GS_OUT
{
  vec3 color;
} gs_out;

void main()
{
  for (int i = 0; i < gl_in.length(); ++i)
  {
    gl_Position = ciModelViewProjection * block.rotationMatrix[gl_InvocationID] * gl_in[i].gl_Position;
    gs_out.color = gs_in[i].color;
    gl_ViewportIndex = gl_InvocationID;
    EmitVertex();
  }
  EndPrimitive();
}
