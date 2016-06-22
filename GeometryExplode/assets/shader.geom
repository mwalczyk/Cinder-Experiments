#version 430

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

uniform mat4 ciModelViewProjection;
uniform float ciElapsedSeconds;

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
  // calculate the face normal by taking the cross product of the triangle's edges
  vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
  vec3 normal = normalize(cross(ab, ac));

  // calculate the centroid
  const vec3 center = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;

  // emit the first vertex at the triangle's center
  gl_Position = ciModelViewProjection * vec4(center, 1.0);
  gs_out.color = vec3(0.0);
  EmitVertex();

  // emit the second vertex at some distance along the face's normal
  gl_Position = ciModelViewProjection * vec4(center + normal * 0.5, 1.0);
  gs_out.color = gs_in[0].color;
  EmitVertex();

  // here, we are being explicit, but the compiler will automatically call EndPrimitive() for us
  EndPrimitive();
}
