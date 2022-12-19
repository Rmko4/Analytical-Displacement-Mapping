#version 410
// Tesselation Control Shader (TCS)
layout(vertices = 4) out;

layout(location = 0) in vec3[] vertcoords_vs;
layout(location = 1) in vec3[] vertnormals_vs;

layout(location = 0) out vec3[] vertcoords_tc;
layout(location = 1) out vec3[] vertnormals_tc;

uniform float innerTessLevel;
uniform float outerTessLevel;

void main() {
  if (gl_InvocationID == 0) {
    gl_TessLevelOuter[0] = outerTessLevel;
    gl_TessLevelOuter[1] = outerTessLevel;
    gl_TessLevelOuter[2] = outerTessLevel;
    gl_TessLevelOuter[3] = outerTessLevel;

    gl_TessLevelInner[0] = innerTessLevel;
    gl_TessLevelInner[1] = innerTessLevel;
  }

  // simply pass through everything. Note that you can also simply use the
  // coords and normals using the layout in the next stage, in which case this
  // statement is not needed.
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  vertcoords_tc[gl_InvocationID] = vertcoords_vs[gl_InvocationID];
  vertnormals_tc[gl_InvocationID] = vertnormals_vs[gl_InvocationID];
}
