#version 430 core

#extension GL_ARB_bindless_texture : require

layout (std140, binding = 0) uniform TextureBlock
{
	sampler2D textures[9];
};

uniform int uIndex;

in VS_OUT
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec3 color;
} fs_in;

out vec4 oColor;

void main()
{
	vec3 samp = texture(textures[uIndex], fs_in.texcoord).rgb;
	vec3 color = fs_in.color;
	oColor = vec4(samp + color, 1.0);
}
