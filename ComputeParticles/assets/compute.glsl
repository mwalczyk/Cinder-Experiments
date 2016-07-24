#version 430

uniform float ciElapsedSeconds;

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

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

vec2 hash(in vec2 p)
{
    p = vec2(dot(p, vec2(12.9898, 78.233)),
             dot(p, vec2(139.234, 98.187)));

    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

vec2 quinticHermine(in vec2 x)
{
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

float noise(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    vec2 a = hash(i + vec2(0.0, 0.0));
    vec2 b = hash(i + vec2(1.0, 0.0));
    vec2 c = hash(i + vec2(0.0, 1.0));
    vec2 d = hash(i + vec2(1.0, 1.0));
    vec2 u = quinticHermine(f);

    float val = mix(mix(dot(a, f - vec2(0.0,0.0)),
                        dot(b, f - vec2(1.0,0.0)), u.x),
                    mix(dot(c, f - vec2(0.0,1.0)),
                        dot(d, f - vec2(1.0,1.0)), u.x), u.y);
    return val;
}

void main()
{
  uint gid = gl_GlobalInvocationID.x;
  vec3 home = particles[gid].home;
  vec3 dirToHome = normalize(home);

  float n = noise(home.yz + home.x * home.z * 2.0 + ciElapsedSeconds * 0.15);

  float offset = n * 4.0;
  float s = sin(ciElapsedSeconds * 1.5 + offset) * 2.5;
  float c = cos(ciElapsedSeconds * 1.5 + offset) * 1.5;

  vec3 next = home + vec3(s, c, fract(s * c));

  //vec3 next = home + dirToHome * n * 3.0 + dirToHome;

  particles[gid].position = next;
  particles[gid].color = vec3(noise(next.xy * 0.2) * 0.5 + 0.2, 0.1, 0.2);
}
