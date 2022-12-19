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
  // Abstract patch coordinates
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  /* Vertices layout (should be transposed, or use cw setting)
  * 15 14 8  11 -> 12 13 14 15
  * 12 13 9  10 -> 8  9  10 11
  * 2  1  5  4  -> 4  5  6  7
  * 3  0  6  7  -> 0  1  2  3
  */

  // Use three control point matrices, one for each spatial dimension.
  mat4 Gx = mat4(vertcoords_tc[15].x, vertcoords_tc[12].x, vertcoords_tc[2].x, vertcoords_tc[3].x,
              vertcoords_tc[14].x, vertcoords_tc[13].x, vertcoords_tc[1].x, vertcoords_tc[0].x,
              vertcoords_tc[8].x, vertcoords_tc[9].x, vertcoords_tc[5].x, vertcoords_tc[6].x,
              vertcoords_tc[11].x, vertcoords_tc[10].x, vertcoords_tc[4].x, vertcoords_tc[7].x);

  mat4 Gy = mat4(vertcoords_tc[15].y, vertcoords_tc[12].y, vertcoords_tc[2].y, vertcoords_tc[3].y,
              vertcoords_tc[14].y, vertcoords_tc[13].y, vertcoords_tc[1].y, vertcoords_tc[0].y,
              vertcoords_tc[8].y, vertcoords_tc[9].y, vertcoords_tc[5].y, vertcoords_tc[6].y,
              vertcoords_tc[11].y, vertcoords_tc[10].y, vertcoords_tc[4].y, vertcoords_tc[7].y);

  mat4 Gz = mat4(vertcoords_tc[15].z, vertcoords_tc[12].z, vertcoords_tc[2].z, vertcoords_tc[3].z,
              vertcoords_tc[14].z, vertcoords_tc[13].z, vertcoords_tc[1].z, vertcoords_tc[0].z,
              vertcoords_tc[8].z, vertcoords_tc[9].z, vertcoords_tc[5].z, vertcoords_tc[6].z,
              vertcoords_tc[11].z, vertcoords_tc[10].z, vertcoords_tc[4].z, vertcoords_tc[7].z);

  // Mt * U or MT * V yields B-splines
  mat4 Mt = mat4(-1, 3, -3, 1,
                3, -6, 3, 0,
                -3, 0, 3, 0,
                1, 4, 1, 0);
  Mt /= 6;

  // Compute position
  vec4 U = vec4(u*u*u, u*u, u, 1);
  vec4 V = vec4(v*v*v, v*v, v, 1);

  float posX = dot(Mt * U, Gx * Mt * V);
  float posY = dot(Mt * U, Gy * Mt * V);
  float posZ = dot(Mt * U, Gz * Mt * V);

  vec3 pos = vec3(posX, posY, posZ);

  // Compute normal using derivatives wrt u and v.
  vec4 dU = vec4(3*u*u, 2*u, 1, 0);
  vec4 dV = vec4(3*v*v, 2*v, 1, 0); 

  float tanUX = dot(Mt * dU, Gx * Mt * V);
  float tanUY = dot(Mt * dU, Gy * Mt * V);
  float tanUZ = dot(Mt * dU, Gz * Mt * V);
  float tanVX = dot(Mt * U, Gx * Mt * dV);
  float tanVY = dot(Mt * U, Gy * Mt * dV);
  float tanVZ = dot(Mt * U, Gz * Mt * dV);

  // Cross product of tangent lines yields normal
  vec3 sU = vec3(tanUX, tanUY, tanUZ);
  vec3 sV = vec3(tanVX, tanVY, tanVZ);
  vec3 normal = normalize(cross(sU, sV));

  // Multiply with matrices to do coordinate transformations
  gl_Position = projectionmatrix * modelviewmatrix * vec4(pos, 1.0);
  vertcoords_te = vec3(modelviewmatrix * vec4(pos, 1.0));
  vertnormals_te = normalize(normalmatrix * normal);
}
