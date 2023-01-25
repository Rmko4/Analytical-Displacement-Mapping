#version 410
// Vertex shader

layout(location = 0) in vec3 vertcoords;
layout(location = 1) in vec3 vertnormal;

layout(location = 0) out vec3 vertcoords_vs;
layout(location = 1) out vec3 vertnormal_vs;
layout(location = 2) out vec2 vertndc_vs;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;

void main() {
  gl_Position = vec4(vertcoords, 1.0);

  vertcoords_vs = vertcoords;
  vertnormal_vs = vertnormal;

  // Computing the x,y-components of the normalized device coordinates (NDC)
  vec4 clipPos = projectionmatrix * modelviewmatrix * gl_Position;
  vertndc_vs = clipPos.xy / clipPos.w;
}
