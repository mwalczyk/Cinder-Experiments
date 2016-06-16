#version 420

in TES_OUT
{
  vec2 texcoord;
  vec3 position;
} fs_in;

in vec3 tesPatchColor;

out vec4 oColor;

void main()
{

#ifdef USE_PATCH_COLOR
  oColor = vec4(tesPatchColor, 1.0);
#else
  oColor = vec4(fs_in.texcoord, 1.0, fs_in.position.y);
#endif
}
