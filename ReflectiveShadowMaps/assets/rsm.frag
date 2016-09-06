#version 430

layout(location = 0) out vec4 o_position; // world space positions
layout(location = 1) out vec4 o_normal;   // world space normals
layout(location = 2) out vec4 o_color;    // emissive

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec3 color;
} fs_in;

void main()
{
  o_position = vec4(fs_in.position, 1.0);
  o_normal = vec4(fs_in.normal, 1.0);
  o_color = vec4(fs_in.color, 1.0);
}
