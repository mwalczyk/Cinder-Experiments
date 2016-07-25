#version 420

layout (quads, fractional_odd_spacing) in;

uniform mat4 ciModelViewProjection;
uniform float ciElapsedSeconds;
uniform float uNoiseIntensity;

in TCS_OUT
{
  vec2 texcoord;
} tes_in[];

out TES_OUT
{
  vec2 texcoord;
  vec3 position;
} tes_out;

patch in vec3 tcsPatchColor;
out vec3 tesPatchColor;

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

    float val = mix( mix( dot( a, f - vec2(0.0,0.0) ),
                          dot( b, f - vec2(1.0,0.0) ), u.x),
                     mix( dot( c, f - vec2(0.0,1.0) ),
                          dot( d, f - vec2(1.0,1.0) ), u.x), u.y);
    return val;
}

void main()
{
  // calculate the texture coordinates and position of the generated vertex by
  // linearly interpolating between the four control point vertices
  vec2 tex1 = mix(tes_in[0].texcoord, tes_in[1].texcoord, gl_TessCoord.x);
  vec2 tex2 = mix(tes_in[2].texcoord, tes_in[3].texcoord, gl_TessCoord.x);

  vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
  vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
  vec4 p = mix(p1, p2, gl_TessCoord.y);

  // displacement
  float t = ciElapsedSeconds;
  float s = sin(t);
  float c = cos(t);
  const float orbitRadius = 0.9;
  vec3 center = vec3(s, 0.0, c) * orbitRadius;
  float d = distance(p.xyz, center);
  p.y += noise(vec2(d) * uNoiseIntensity + ciElapsedSeconds) * 0.5;

  // pass attributes to fragment shader
  tes_out.texcoord = mix(tex1, tex2, gl_TessCoord.y);
  tes_out.position = p.xyz;
  tesPatchColor = tcsPatchColor;

  gl_Position = ciModelViewProjection * p;
}
