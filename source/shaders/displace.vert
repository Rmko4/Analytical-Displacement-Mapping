#version 410
// Vertex shader

layout(location = 0) in vec3 vertcoords;
layout(location = 1) in vec3 vertnormal;

layout(location = 0) out vec3 vertcoords_vs;
layout(location = 1) out vec3 vertnormal_vs;
layout(location = 2) out vec2 vertndc_vs;

out float tessFactor;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;

uniform float tessDetail;

void main() {
  gl_Position = vec4(vertcoords, 1.0);
  vec4 clipPos = projectionmatrix * modelviewmatrix * gl_Position;
  vertndc_vs = clipPos.xy / clipPos.w;
  // float z = clipPos.z;
  // z /= clipPos.w;

  // tessFactor = tessDetail * (1.0 - z);

  vertcoords_vs = vertcoords;
  vertnormal_vs = vertnormal;
}
