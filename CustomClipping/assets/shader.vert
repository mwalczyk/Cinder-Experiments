#version 420

#define USE_NOISE

uniform mat4 ciModelView;
uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;
uniform float ciElapsedSeconds;
uniform int uLayer;
uniform float uNoiseSpeed = 0.5;

// custom clip distances
uniform vec4 uClipSphere = vec4(0.6, 0.0, 0.0, 0.6);

in vec4 ciPosition;
in vec3 ciNormal;

out VS_OUT
{
  vec3 position;
  vec3 normal;
} vs_out;

vec2 hash2(in vec2 p)
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

    vec2 a = hash2(i + vec2(0.0, 0.0));
    vec2 b = hash2(i + vec2(1.0, 0.0));
    vec2 c = hash2(i + vec2(0.0, 1.0));
    vec2 d = hash2(i + vec2(1.0, 1.0));
    vec2 u = quinticHermine(f);

    float val = mix(mix(dot(a, f - vec2(0.0,0.0)),
                        dot(b, f - vec2(1.0,0.0)), u.x),
                    mix(dot(c, f - vec2(0.0,1.0)),
                        dot(d, f - vec2(1.0,1.0)), u.x), u.y);
    return val;
}

void main()
{
  float noiseOffset = uLayer * 200.0;
  float noiseScale = (uLayer / 16.0) * 12.0;
  vec2 noiseSeed = ciPosition.zz * ciPosition.z * noiseScale + noiseOffset;
  float n = noise(ciElapsedSeconds * uNoiseSpeed + noiseSeed);

#ifdef USE_NOISE
  gl_ClipDistance[0] = n;
#else
  gl_ClipDistance[0] = length(ciPosition.xyz / ciPosition.w - uClipSphere.xyz) - uClipSphere.w;
#endif
  vec3 dp = ciPosition.xyz + ciNormal * n * 0.1;

  vs_out.position = dp;
  vs_out.normal = ciNormalMatrix * ciNormal;

  gl_Position = ciModelViewProjection * vec4(dp, 1.0);
}
