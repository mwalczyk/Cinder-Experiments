#version 420

uniform int uLayer;

out vec4 oColor;

in VS_OUT
{
  vec3 position;
  vec3 normal;
} fs_in;

void main()
{
  // note that this variable is also available in the fragment shader
  // it will be a positive, since all vertices with negative clip distances
  // do not make it to the rasterizer
  float d = gl_ClipDistance[0];

  float l = length(fs_in.position);

  oColor = vec4(fs_in.normal * 0.5 + d + (uLayer / 16.0) * 0.3, l);
}
