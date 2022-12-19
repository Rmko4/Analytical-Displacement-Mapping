#version 410
layout(quads, equal_spacing, ccw) in;

layout(location = 0) in vec3[] vertcoords_tc;
layout(location = 1) in vec3[] vertnormals_tc;

layout(location = 0) out vec3 vertcoords_te;
layout(location = 1) out vec3 vertnormals_te;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  // Bilinear interpolation
  vec3 pos = mix(mix(vertcoords_tc[0], vertcoords_tc[1], u),
                 mix(vertcoords_tc[3], vertcoords_tc[2], u), v);

  vec3 normal = mix(mix(vertnormals_tc[0], vertnormals_tc[1], u),
                    mix(vertnormals_tc[3], vertnormals_tc[2], u), v);

  gl_Position = projectionmatrix * modelviewmatrix * vec4(pos, 1.0);
  vertcoords_te = vec3(modelviewmatrix * vec4(pos, 1.0));
  vertnormals_te = normalize(normalmatrix * normal);
}
