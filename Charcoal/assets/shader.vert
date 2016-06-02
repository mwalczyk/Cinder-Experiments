#version 420

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec3 ciTexCoord0;

out VS_OUT
{
	vec3 position;
  vec2 uv;
} vs_out;

void main()
{
	vs_out.position = ciPosition.xyz;
  vs_out.uv = ciTexCoord0.st;
	gl_Position = ciModelViewProjection * ciPosition;
}
