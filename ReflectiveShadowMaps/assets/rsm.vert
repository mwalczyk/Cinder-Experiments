#version 430

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec3 ciColor;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 color;
} vs_out;

void main()
{
  vs_out.position = (ciModelMatrix * ciPosition).xyz;
  vs_out.normal = ciNormalMatrix * ciNormal;
  vs_out.color = ciColor;

  gl_Position = ciModelViewProjection * ciPosition;
}
