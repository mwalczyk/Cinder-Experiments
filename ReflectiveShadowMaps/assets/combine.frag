#version 430

layout(binding = 0) uniform sampler2D u_position;
layout(binding = 1) uniform sampler2D u_normal;
layout(binding = 2) uniform sampler2D u_color;
layout(binding = 3) uniform sampler2D u_depth;

uniform float ciElapsedSeconds;

out vec4 o_color;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 color;
		vec4 shadow_coord;
} fs_in;

vec2 seed = gl_FragCoord.xy * (float(ciElapsedSeconds * 0.001) + 1.0);

vec2 rand2n()
{
    seed += vec2(-1.0, 1.0);
	// implementation based on: lumina.sourceforge.net/Tutorials/Noise.html
    return vec2(fract(sin(dot(seed.xy, vec2(12.9898,78.233))) * 43758.5453),
								fract(cos(dot(seed.xy, vec2(4.898,7.23))) * 23421.631));
}

float in_shadow(in vec4 shadow_coord)
{
		vec3 proj_coords = (shadow_coord.xyz / shadow_coord.w) * 0.5 + 0.5;
		float closest_depth = texture(u_depth, proj_coords.xy).r;
		float current_depth = proj_coords.z;
		const float bias = 0.005;
		return current_depth - bias > closest_depth ? 1.0 : 0.0;
}

vec3 rsm(in vec3 p, in vec3 n)
{
	vec3 texture_space_position = (fs_in.shadow_coord.xyz / fs_in.shadow_coord.w) * 0.5 + 0.5;
	vec3 indirect_illumination = vec3(0.0);
	float r_max = 0.53;

	const int sample_count = 100;
	for (int i = 0; i < sample_count; ++i)
	{
		vec2 rnd = rand2n();
		vec2 coords = texture_space_position.xy + r_max * rnd;
		vec3 rms_position = texture(u_position, coords).rgb;
		vec3 rms_normal = texture(u_normal, coords).rgb;
		vec3 rms_flux = texture(u_color, coords).rgb;

		float numerator = max(0.0, dot(rms_normal, p - rms_position)) *
											max(0.0, dot(n, rms_position - p));
		float denominator = pow(length(p - rms_position), 4.0);
		vec3 result = rms_flux * (numerator / denominator);
		result *= rnd.x * rnd.x;
		indirect_illumination += result;
	}
	return max(vec3(0.0), indirect_illumination * 4.0);
}

void main()
{
	//float shadow = 1.0 - in_shadow(fs_in.shadow_coord);
	vec3 rsm_contribution = rsm(fs_in.position, fs_in.normal);
  o_color = vec4(rsm_contribution, 1.0);
}
