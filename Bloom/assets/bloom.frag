#version 430

layout (location = 0) out vec4 oColor0; // normal
layout (location = 1) out vec4 oColor1; // luminance

in VS_OUT
{
  vec3 position;
  vec3 normal;
} fs_in;

void main()
{
  const vec3 lightPos = vec3(1.0, 2.0, 10.0);
  vec3 L = normalize(lightPos - fs_in.position);
  vec3 N = normalize(fs_in.normal);
  vec3 R = reflect(-L, N);
  vec3 V = normalize(-fs_in.position);

  float d = max(dot(N, L), 0.0);
  float s = 0.0;

  if (d > 0.0)
  {
    s = pow(max(dot(R, V), 0.0), 16.0);
  }

  float a = 0.05;

  vec3 sphereColor = vec3(1.0, 0.0, 0.2) * d + vec3(1.0) * s + vec3(a);
  float y = dot(sphereColor, vec3(0.299, 0.587, 0.144));
  vec3 lumaColor = sphereColor * 4.0 * smoothstep(0.8, 1.2, y);

  oColor0 = vec4(sphereColor, 1.0);
  oColor1 = vec4(lumaColor, 1.0);
}
