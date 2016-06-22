#version 430

layout (binding = 0) uniform sampler2DArray uLayerTexture;
uniform int uLayerIndex;

in VS_OUT
{
  vec2 texcoord;
} fs_in;

out vec4 oColor;

void main()
{
  oColor = texture(uLayerTexture, vec3(fs_in.texcoord, uLayerIndex));
}
