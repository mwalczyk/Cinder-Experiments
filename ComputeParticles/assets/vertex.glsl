#version 430

uniform mat4 ciModelViewProjection;

layout(location = 0) in int vIndex;

struct Particle
{
  vec3 position;
  vec3 home;
  vec3 color;
};

layout(std140, binding = 0) buffer sPositions
{
    Particle particles[];
};

out VS_OUT
{
  vec3 color;
} vs_out;

void main()
{
  gl_Position = ciModelViewProjection * vec4(particles[vIndex].position, 1.0);

  float distFromHome = distance(particles[vIndex].position, particles[vIndex].home);
  float distShaped = pow(cos(3.14159 * distFromHome * 0.5), 4.0); // exp was 2

  gl_PointSize = distShaped * 6.0; // was 16

  vs_out.color = particles[vIndex].color + distShaped * 0.5;
}
