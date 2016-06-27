#version 430

layout (local_size_x = 32, local_size_y = 32) in;
layout (binding = 0, rgba32f) uniform readonly image2D uImgInput;
layout (binding = 1) uniform writeonly image2D uImgOutput;

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
  ivec2 p = ivec2(gl_GlobalInvocationID.xy);
  texel = imageLoad(uImgInput, p);
  texel = vec4(vec3(1.0) - texel.rgb, 1.0);
  imageStore(uImgOutput, p, texel);
}
