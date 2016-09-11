#version 430

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;
uniform mat3 ciNormalMatrix;

uniform mat4 u_shadow_matrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec3 ciColor;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec4 shadow_coord;
} vs_out;

void main()
{
  vs_out.position = (ciModelMatrix * ciPosition).xyz;
  vs_out.normal = ciNormalMatrix * ciNormal;
  vs_out.color = ciColor;
  vs_out.shadow_coord = u_shadow_matrix * vec4(vs_out.position.xyz, 1.0);

  gl_Position = ciModelViewProjection * ciPosition;
}
