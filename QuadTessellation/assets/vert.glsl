#version 420

out VS_OUT
{
  vec2 texcoord;
} vs_out;

void main()
{
  const vec4 vertices[] = {
    vec4(-0.5, 0.0, -0.5, 1.0),
    vec4( 0.5, 0.0, -0.5, 1.0),
    vec4(-0.5, 0.0, 0.5, 1.0),
    vec4( 0.5, 0.0, 0.5, 1.0)
  };

  vs_out.texcoord = vertices[gl_VertexID].xz + vec2(0.5);
  gl_Position = vertices[gl_VertexID];
}
