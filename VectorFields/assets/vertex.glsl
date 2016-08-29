#version 430

uniform mat4 ciModelMatrix;
uniform mat4 ciViewMatrix;
uniform mat4 ciProjectionMatrix;

in mat4 v_instance_transform;
in vec3 v_instance_color;

in vec4 ciPosition;

out VS_OUT
{
	vec3 color;
} vs_out;

void main()
{
	vs_out.color = v_instance_color;

	gl_Position = ciProjectionMatrix * ciViewMatrix * ciModelMatrix * v_instance_transform * ciPosition;
}
