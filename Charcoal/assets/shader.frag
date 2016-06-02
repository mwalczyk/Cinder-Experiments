#version 420

uniform float uTime;
uniform sampler2D uDrawing;

in VS_OUT
{
  vec3 position;
  vec2 uv;
} fs_in;

out vec4 oColor;

vec2 hash(in vec2 p)
{
    p = vec2(dot(p, vec2(12.9898, 78.233)),
             dot(p, vec2(139.234, 98.187)));

    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in vec2 p)
{
	// integer and fractional parts of p
    vec2 i = floor(p);
    vec2 f = fract(p);

    // four corners of square
    vec2 a = hash(i + vec2(0.0, 0.0));
    vec2 b = hash(i + vec2(1.0, 0.0));
    vec2 c = hash(i + vec2(0.0, 1.0));
    vec2 d = hash(i + vec2(1.0, 1.0));

    // interpolant
    vec2 u = smoothstep(0.0, 1.0, f);

   	// mix a->b, mix c->d, then mix the two results
    float val = mix(mix(dot(a, f - vec2(0.0,0.0)),
                        dot(b, f - vec2(1.0,0.0)), u.x),
                    mix(dot(c, f - vec2(0.0,1.0)),
                        dot(d, f - vec2(1.0,1.0)), u.x), u.y);

    // rescale to 0...1
    return val * 0.5 + 0.5;
}

float fbm(in vec2 p)
{
	// fractal noise is constructed from layers or "octaves"
	// of Perlin noise with increasing frequency and decreasing
	// amplitude
	float n1 = noise(p         + uTime * 0.05) * 0.75;
	float n2 = noise(p * 2.0   + uTime * 0.01) * 0.375;
	float n3 = noise(p * 4.0   + uTime * 0.01) * 0.1875;
	float n4 = noise(p * 8.0   + uTime * 4.0) * 0.09375;
	float n5 = noise(p * 16.0  + uTime * 4.0) * 0.046875;
	return n1 + n2 + n3 + n4 + n5;
}

void main()
{
  // noise calculations
  const float noiseFreq = 20.0;
  float n1 = fbm(fs_in.uv * noiseFreq);
  float n2 = fbm(fs_in.uv * noiseFreq + 1000.0);
  float no = hash(gl_FragCoord.xy + uTime).r;
  vec3 staticNoise = vec3(pow(no, 3.0) * 0.05);

  // displacement
  vec4 samp = texture(uDrawing, fs_in.uv + vec2(n1, n2) * 0.05);

  float s = noise(gl_FragCoord.xy + uTime);

  oColor = vec4(samp.rgb + staticNoise, 1.0);
}
