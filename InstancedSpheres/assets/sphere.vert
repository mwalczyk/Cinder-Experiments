#version 420

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec3 ciColor;
in mat4 iModelMatrix;

out vec3 vColor;

void main()
{
  vColor = ciColor;
  gl_Position = ciModelViewProjection * iModelMatrix * ciPosition;
}
