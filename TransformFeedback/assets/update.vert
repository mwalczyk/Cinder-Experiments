#version 420

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in ivec4 connection;

layout (binding = 0) uniform samplerBuffer uTexPosition;

// varyings that will be captured by the transform feedback process
out vec4 tfPosition;
out vec3 tfVelocity;

uniform float t = 0.07; // time step
uniform float k = 7.1;  // spring constant
uniform float c = 2.8;  // damping constant
uniform float restLength = 0.88;

const vec3 gravity = vec3(0.0, -0.08, 0.0);

void main()
{
  vec3 p = position.xyz;    // position
  float m = position.w;     // mass
  vec3 u = velocity;        // initial velocity

  vec3 f = gravity * m - c * u;

  bool fixedNode = true;

  for (int i = 0; i < 4; ++i)
  {
    if(connection[i] != -1)
    {
      vec3 q = texelFetch(uTexPosition, connection[i]).xyz;
      vec3 d = q - p;
      float x = length(d);
      f += -k * (restLength - x) * normalize(d);
      fixedNode = false;
    }
  }

  if (fixedNode)
  {
    f = vec3(0.0);
  }

  vec3 acceleration = f / m;
  vec3 displacement = u * t + 0.5 * acceleration * t * t;
  vec3 finalVelocity = u + acceleration * t;

  displacement = clamp(displacement, vec3(-25.0), vec3(25.0));

  // transform feedback varyings
  tfPosition = vec4(p + displacement, m);
  tfVelocity = finalVelocity;
}
