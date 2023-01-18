#version 410
// Tesselation Control Shader (TCS)
layout(vertices = 16) out;

layout(location = 0) in vec3[] vertcoords_vs;
layout(location = 1) in vec3[] vertnormals_vs;
layout(location = 2) in vec2[] vertndc_vs;

in float[] tessFactor;

layout(location = 0) out vec3[] vertcoords_tc;
layout(location = 1) out vec3[] vertnormals_tc;


uniform bool dynamicLoD;
uniform float tessDetail;

uniform float innerTessLevel;
uniform float outerTessLevel;

void computeDymamicLoD() {

}

void main() {
  if (gl_InvocationID == 0) {
    if (dynamicLoD) {
      /* default (u,v) layout of quad
      * (0,1) (1,1) -> 6 10
      * (0,0) (1,0) -> 5 9
      */
      gl_TessLevelOuter[0] = max(tessFactor[6], tessFactor[5]);
      gl_TessLevelOuter[1] = max(tessFactor[5], tessFactor[9]);
      gl_TessLevelOuter[2] = max(tessFactor[9], tessFactor[10]);
      gl_TessLevelOuter[3] = max(tessFactor[10], tessFactor[6]);

      gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
      gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
    } else {
      gl_TessLevelOuter[0] = outerTessLevel;
      gl_TessLevelOuter[1] = outerTessLevel;
      gl_TessLevelOuter[2] = outerTessLevel;
      gl_TessLevelOuter[3] = outerTessLevel;

      gl_TessLevelInner[0] = innerTessLevel;
      gl_TessLevelInner[1] = innerTessLevel;
    }
  }

  // simply pass through everything. Note that you can also simply use the
  // coords and normals using the layout in the next stage, in which case this
  // statement is not needed.
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  vertcoords_tc[gl_InvocationID] = vertcoords_vs[gl_InvocationID];
  vertnormals_tc[gl_InvocationID] = vertnormals_vs[gl_InvocationID];
}
