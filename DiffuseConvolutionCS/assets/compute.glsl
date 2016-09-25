#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform readonly imageCube u_input_cubemap;
layout (binding = 1) uniform writeonly imageCube u_output_cubemap;

vec3 normals[6] = {
  vec3(1.0, 0.0, 0.0),  // +X
  vec3(-1.0, 0.0, 0.0), // -X
  vec3(0.0, 1.0, 0.0),  // +Y
  vec3(0.0, -1.0, 0.0), // -Y
  vec3(0.0, 0.0, 1.0),  // +Z
  vec3(0.0, 0.0, -1.0)  // -Z
};

vec3 get_direction(in ivec3 p, in ivec2 sz)
	{
	  vec3 direction;
    int face_id = p.z;
    int face_size = sz.x;

		if (face_id == 0)
		{
			direction = vec3(0.5f, -((p.y + 0.5f) / face_size - 0.5f), -((p.x + 0.5f) / face_size - 0.5f));
		}
		else if (face_id == 1)
		{
			direction = vec3(-0.5f, -((p.y + 0.5f) / face_size - 0.5f), ((p.x + 0.5f) / face_size - 0.5f));
		}
		else if (face_id == 2)
		{
			direction = vec3(((p.x + 0.5f) / face_size - 0.5f), 0.5f, ((p.y + 0.5f) / face_size - 0.5f));
		}
		else if (face_id == 3)
		{
			direction = vec3(((p.x + 0.5f) / face_size - 0.5f), -0.5f, -((p.y + 0.5f) / face_size - 0.5f));
		}
		else if (face_id == 4)
		{
			direction = vec3(((p.x + 0.5f) / face_size - 0.5f), -((p.y + 0.5f) / face_size - 0.5f), 0.5f);
		}
		else if (face_id == 5)
		{
			direction = vec3(-((p.x + 0.5f) / face_size - 0.5f), -((p.y + 0.5f) / face_size - 0.5f), -0.5f);
		}
		return direction;
}

vec4 convolve(in ivec3 p)
{
    ivec2 xy = p.xy;
    int face_id = p.z;

    // +X, -X, +Y, -Y, +Z, -Z
    vec3 face_normal = normals[face_id];

    // note:
    // the image coordinate for cube map and cube map arrays are both ivec3​
    // the coordinate is not a direction - it is a texel coordinate within the space of the cube
    // the third component is the face index
    ivec2 input_size = imageSize(u_input_cubemap);
    ivec2 output_size = imageSize(u_output_cubemap);

    return vec4(1.0);
}

void main()
{
  // built-in inputs to the compute shader:
  // gl_LocalInvocationID (uvec3): the index of the shader invocation within the local work group
  // gl_WorkGroupSize (uvec3): the size of the local work group - unused dimensions will have a value of 1
  // gl_WorkGroupID (uvec3): the index of the shader invocation within the global work group
  // gl_NumWorkGroups (uvec3): set by the application with glDispatchCompute()
  // gl_GlobalInvocationID (uvec3): the absolute index of the shader invocation within all shader invocations

  // note that gl_GlobalInvocationID is calculated as follows:
  // gl_GlobalInvocationID = gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID

  // logically invert the first image and store it in the second
  vec4 texel;

  // no filtering is performed when reading/writing to images - we use integer indices
  // here, p.xy are the xy coordinates of the texel, relative to a specific face (from 0 to the input_size)
  // and p.z is the face ID (see below)
  ivec3 p = ivec3(gl_GlobalInvocationID.xyz);

  //vec4 result = convolve(p);

  // face ordering (in terms of horizontal cross layout):
  //   2
  // 1 4 0 5
  //   3
  if (p.x < input_size.x * 0.5)
  {
    texel = imageLoad(u_input_cubemap, p);
    texel = vec4(vec3(1.0) - texel.rgb, 1.0);
    imageStore(u_output_cubemap, p, texel);
  }
  else
  {
    imageStore(u_output_cubemap, p, vec4(1.0, 0.0, 0.0, 1.0));
  }
}
