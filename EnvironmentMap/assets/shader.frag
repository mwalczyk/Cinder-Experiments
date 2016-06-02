#version 420

layout (binding = 0) uniform samplerCube uEnvironmentMap;

uniform mat4 ciViewMatrixInverse;

out vec4 oColor;

in VS_OUT
{
  vec3 normal;
  vec3 position;
} fs_in;

void main()
{
  vec3 e = normalize(fs_in.position);                 // direction vector pointing from the eye to the current fragment
  vec3 r_view = reflect(e, normalize(fs_in.normal));  // reflection of the above vector about the surface normal
  vec3 r_world = (ciViewMatrixInverse * vec4(r_view, 0.0)).xyz;

  vec4 c = textureCube(uEnvironmentMap, r_world);
  
  oColor = c;
}
