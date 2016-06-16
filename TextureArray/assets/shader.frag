#version 420

uniform sampler2DArray uSprites;

in VS_OUT
{
		flat int index;
		vec2 texCoord;
} fs_in;

out vec4 oColor;

void main()
{
  vec4 color = texture(uSprites, vec3(fs_in.texCoord, float(fs_in.index)));
  oColor = color;
}
