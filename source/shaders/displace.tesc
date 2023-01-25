#version 410
// Tesselation Control Shader (TCS)
layout(vertices = 16) out;

layout(location = 0) in vec3[] vertcoords_vs;
layout(location = 1) in vec3[] vertnormals_vs;
layout(location = 2) in vec2[] vertndc_vs;

layout(location = 0) out vec3[] vertcoords_tc;
layout(location = 1) out vec3[] vertnormals_tc;

uniform bool dynamicLoD;
uniform float tessDetail;

uniform float tileSize;

// Distance between to vertices in screen space
float distance(int x, int y) {
  return length(vertndc_vs[x] - vertndc_vs[y]);
}

// Computes TL_v: Tessellation level of vertex
// given the four adjacent vertices.
float TL_v(int vert, int left, int bottom, int right, int top) {
  float maxLength = distance(vert, left);
  maxLength = max(maxLength, distance(vert, bottom));
  maxLength = max(maxLength, distance(vert, right));
  maxLength = max(maxLength, distance(vert, top));
  return clamp(tessDetail * maxLength, 1.F, 64.F);
}

// Computes TL_e: Tessellation level of edge
// given two adjacent vertices.
float TL_e(int left, int right) {
  return clamp(tessDetail * distance(left, right), 1.F, 64.F);
} 

void main() {
  if (gl_InvocationID == 0) {
    if (dynamicLoD) {
      /* default (u,v) layout of corner vertices of patch
      * (0,1) (1,1) -> 9 10 -> D A 
      * (0,0) (1,0) -> 5 6  -> C B 
      */

      float TL_vA = TL_v(10, 6, 9, 14, 11);
      float TL_vB = TL_v(6, 2, 5, 10, 7);
      float TL_vC = TL_v(5, 1, 4, 9, 6);
      float TL_vD = TL_v(9, 5, 8, 13, 10);

      gl_TessLevelOuter[0] = max(TL_vD, TL_vC);
      gl_TessLevelOuter[1] = max(TL_vC, TL_vB);
      gl_TessLevelOuter[2] = max(TL_vB, TL_vA);
      gl_TessLevelOuter[3] = max(TL_vA, TL_vD);

      gl_TessLevelInner[0] = max(TL_e(5, 6), TL_e(9, 10));
      gl_TessLevelInner[1] = max(TL_e(5, 9), TL_e(6, 10));
    } else {
      gl_TessLevelOuter[0] = tileSize;
      gl_TessLevelOuter[1] = tileSize;
      gl_TessLevelOuter[2] = tileSize;
      gl_TessLevelOuter[3] = tileSize;

      gl_TessLevelInner[0] = tileSize;
      gl_TessLevelInner[1] = tileSize;
    }
  }

  // Variable pass through.
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  vertcoords_tc[gl_InvocationID] = vertcoords_vs[gl_InvocationID];
  vertnormals_tc[gl_InvocationID] = vertnormals_vs[gl_InvocationID];
}
