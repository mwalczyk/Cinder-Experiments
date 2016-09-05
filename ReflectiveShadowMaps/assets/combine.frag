#version 430

layout(binding = 0) uniform sampler2D u_position;
layout(binding = 1) uniform sampler2D u_normal;
layout(binding = 2) uniform sampler2D u_color;
layout(binding = 3) uniform sampler2D u_depth;

out vec4 o_color;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 color;
		vec4 shadow_coord;
} fs_in;

float in_shadow(in vec4 shadow_coord)
{
		vec3 proj_coords = (shadow_coord.xyz / shadow_coord.w) * 0.5 + 0.5;
		float closest_depth = texture(u_depth, proj_coords.xy).r;
		float current_depth = proj_coords.z;
		const float bias = 0.005;
		return current_depth - bias > closest_depth ? 1.0 : 0.0;
}

void main()
{
	float shadow = 1.0 - in_shadow(fs_in.shadow_coord);

	vec3 proj_coords = (fs_in.shadow_coord.xyz / fs_in.shadow_coord.w) * 0.5 + 0.5;
	vec4 position = texture(u_position, proj_coords.xy);

  o_color = position;//vec4(fs_in.color * shadow, 1.0);
}
