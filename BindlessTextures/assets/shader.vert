#version 430 core

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec3 ciNormal;
in vec3 ciColor;

out VS_OUT
{
		vec3 position;
		vec2 texcoord;
		vec3 normal;
		vec3 color;
} vs_out;

void main()
{
	vs_out.position = ciPosition.xyz;
	vs_out.texcoord = ciTexCoord0;
	vs_out.normal = ciNormal;
	vs_out.color = ciColor;

	gl_Position = ciModelViewProjection * ciPosition;
}
