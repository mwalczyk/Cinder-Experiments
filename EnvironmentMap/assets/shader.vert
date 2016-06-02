#version 420

uniform mat4 ciModelView;
uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;

out VS_OUT
{
  vec3 normal;
  vec3 position;
} vs_out;

void main()
{
  vs_out.position = (ciModelView * ciPosition).xyz;   // vertex position in view space
  vs_out.normal = ciNormalMatrix * ciNormal;          // vertex normal in view space

  gl_Position = ciModelViewProjection * ciPosition;
}
