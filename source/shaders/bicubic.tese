#version 410
layout(quads, equal_spacing, ccw) in;

layout(location = 0) in vec3[] vertcoords_tc;
layout(location = 1) in vec3[] vertnormals_tc;

layout(location = 0) out vec3 vertcoords_te;
layout(location = 1) out vec3 vertnormals_te;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

const mat4 cubicM = mat4(-1, 3, -3, 1,
                            3, -6, 3, 0,
                            -3, 0, 3, 0,
                            1, 4, 1, 0) / 6;

vec3 tensorAccumulatePatch(vec4 x, vec4 y) {
  vec3 res = vec3(0.F);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      res += x[i] * y[j] * vertcoords_tc[4*j + i];
    }
  }

  return res;
}

void main() {
  // Abstract patch coordinates
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  // The cubic basis functions
  vec4 B3u = cubicM * vec4(u*u*u, u*u, u, 1);
  vec4 B3v = cubicM * vec4(v*v*v, v*v, v, 1);

  // The partials of cubic basis functions
  vec4 dB3du = cubicM * vec4(3*u*u, 2*u, 1, 0);
  vec4 dB3dv = cubicM * vec4(3*v*v, 2*v, 1, 0);

  // Bicubic surface evaluation
  vec3 pos = tensorAccumulatePatch(B3u, B3v);

  // Partials of bicubic surface patch
  vec3 dsdu = tensorAccumulatePatch(dB3du, B3v);
  vec3 dsdv = tensorAccumulatePatch(B3u, dB3dv);

  // Normal of bicubic surface patch
  vec3 Ns = normalize(cross(dsdu, dsdv));

  // Multiply with matrices to do coordinate transformations
  gl_Position = projectionmatrix * modelviewmatrix * vec4(pos, 1.0);
  vertcoords_te = vec3(modelviewmatrix * vec4(pos, 1.0));
  vertnormals_te = normalize(normalmatrix * Ns);
}
