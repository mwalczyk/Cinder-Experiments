#version 420

layout(location = 0) in int index;

uniform mat4 ciModelViewProjection;

out VS_OUT
{
		flat int index;
		vec2 texCoord;
} vs_out;

struct droplet_t
{
		float xOffset;
		float yOffset;
		float orientation;
		float unused;
};

layout(std140, binding = 0) uniform droplets
{
  droplet_t droplet[100];
};

void main()
{
	const float sz = 0.1;
	const vec2[4] positions = vec2[4](vec2(-sz, -sz),
																		vec2(sz, -sz),
																		vec2(-sz, sz),
																		vec2(sz, sz));
	vs_out.index = index % 3;

	vec2 displacePosition = positions[gl_VertexID];
	vs_out.texCoord = displacePosition.xy + vec2(0.5);

	displacePosition.x += droplet[index].xOffset;
	displacePosition.y += droplet[index].yOffset;

	gl_Position = vec4(displacePosition, 0.5, 1.0);
}
