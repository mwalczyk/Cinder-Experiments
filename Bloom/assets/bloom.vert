#version 430

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;

out VS_OUT
{
  vec3 position;
  vec3 normal;
} vs_out;

void main()
{
  vs_out.position = (ciModelView * ciPosition).xyz;
  vs_out.normal = ciNormalMatrix * ciNormal;

  gl_Position = ciModelViewProjection * ciPosition;
}
