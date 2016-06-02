#version 420

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;

out VS_OUT
{
	vec3 position;
} vs_out;

void main()
{
	vs_out.position = ciPosition.xyz;
	gl_Position = ciModelViewProjection * ciPosition;
}
