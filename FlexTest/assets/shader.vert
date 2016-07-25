#version 430

uniform mat4 ciModelViewProjection;
in vec4 ciPosition;
in vec4 ciInstancePosition;
in vec3 ciInstanceVelocity;
in vec3 ciColor;
in vec3 ciNormal;

out VS_OUT
{
  vec3 color;
} vs_out;

void main()
{
  const vec3 lightPos = vec3(10.0);
  vec3 p = ciPosition.xyz + ciInstancePosition.xyz;
  vec3 l = normalize(lightPos - p);
  vec3 n = normalize(ciNormal);
  float diff = max(0.0, dot(n, l));

  vs_out.color = normalize(ciInstancePosition.xyz - ciPosition.xyz) * 0.5 + 0.5;
  float v = length(ciInstanceVelocity) * 0.5;
  vs_out.color = vec3(v * 0.1, v * 0.1, v *0.4) * diff;

  gl_Position = ciModelViewProjection * vec4(p, 1.0);
}
