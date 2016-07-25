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

<<<<<<< HEAD
  vec3 colorFromAttrib = vec3(fs_in.position.rg * 0.5, d);

  oColor = vec4(colorFromAttrib + d + (uLayer / 16.0) * 0.5, 1.0);
=======
#ifdef USE_COLOR
  oColor = vec4(fs_in.normal * 0.5 + d + (uLayer / 16.0) * 0.3, l);
#else
  oColor = vec4(vec3(uLayer / 16.0), 1.0);
#endif
>>>>>>> fcab395f8752b6e882fdf8be670c2caac31dec4f
}
