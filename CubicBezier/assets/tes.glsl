#version 420

layout (quads, equal_spacing) in;

uniform mat4 ciProjectionMatrix;
uniform mat3 ciNormalMatrix;

out TES_OUT
{
  vec3 normal;
} tes_out;

vec4 quadraticBezier(vec4 a, vec4 b, vec4 c, float t)
{
    vec4 d = mix(a, b, t);
    vec4 e = mix(b, c, t);
    return mix(d, e, t);
}

vec4 cubicBezier(vec4 a, vec4 b, vec4 c, vec4 d, float t)
{
  vec4 e = mix(a, b, t);
  vec4 f = mix(b, c, t);
  vec4 g = mix(c, d, t);
  return quadraticBezier(e, f, g, t);
}

vec4 evalPatch(vec2 at)
{
  vec4 p[4];
  for(int i = 0; i < 4; ++i)
  {
    p[i] = cubicBezier(gl_in[i + 0].gl_Position,
                       gl_in[i + 4].gl_Position,
                       gl_in[i + 8].gl_Position,
                       gl_in[i + 12].gl_Position,
                       at.y);
  }
  return cubicBezier(p[0], p[1], p[2], p[3], at.x);
}

const float epsilon = 0.001;

void main()
{
  vec4 p1 = evalPatch(gl_TessCoord.xy);
  vec4 p2 = evalPatch(gl_TessCoord.xy + vec2(0.0, epsilon));
  vec4 p3 = evalPatch(gl_TessCoord.xy + vec2(epsilon, 0.0));

  vec3 v1 = normalize(p2.xyz - p1.xyz);
  vec3 v2 = normalize(p3.xyz - p1.xyz);
  tes_out.normal = cross(v1, v2);

  gl_Position = ciProjectionMatrix * p1;
}
