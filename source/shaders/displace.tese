#version 410

#define M_PI 3.1415926538

layout(quads, fractional_even_spacing, ccw) in;

layout(location = 0) in vec3[] vertcoords_tc;
layout(location = 1) in vec3[] vertnormals_tc;

layout(location = 0) out vec3 vertcoords_te;
layout(location = 1) out vec3 vertnormals_te;
layout(location = 2) out vec3 vertbasesurfacedu_te;
layout(location = 3) out vec3 vertbasesurfacedv_te;
layout(location = 4) out vec3 vertbasenormal;

out float vertU;
out float vertV;

out float tileSize;

out float vertdisplacement;
out vec3 vertbasenormaldu;
out vec3 vertbasenormaldv;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

uniform float innerTessLevel;
uniform float outerTessLevel;

uniform float tess_amplitude;

uniform int displacement_mode;

const float freq = .5F;
// const float amplitude = -.1F;

const mat4 bicubicMt = mat4(-1, 3, -3, 1,
                            3, -6, 3, 0,
                            -3, 0, 3, 0,
                            1, 4, 1, 0) / 6;

const mat3 biquadraticMt = mat3(1, -2,  1,
                                -2,  2,  0,
                                 1,  1,  0) / 2;

float subpatchTransform(float t) {
  return fract(innerTessLevel * t - 0.5);
}

float coeff(float u, float v) {
  // Forcing turnable symetry around 0.5, 0.5
  u = mod(u, 1.);
  v = mod(v, 1.);

  if (v <= u && 1. - u < v) {
    float tmp = u;
    u = v;
    v = 1. - tmp;
  }
  else if (v > u && 1. - u <= v)
  {
    u = 1. - u;
    v = 1. - v;
  }
  else if (v >= u && 1. - u > v)
  {
    float tmp = u;
    u = 1. - v;
    v = tmp;
  }

  if (u > 0.5) // This is more than needed: also making each of the 4 triangle into two mirrored right angle triangles
    u = 1. - u;

  switch(displacement_mode)
  {
    case 0:
      return tess_amplitude * sin(2 * M_PI * freq * u) * sin(2 * M_PI * freq * v);
    case 1:
      if (v > 0.4501) return 2 * tess_amplitude;
      return min(1.0, v * 10.0) * tess_amplitude - tess_amplitude;
    case 2:
      return min(1.0, v * 5.0) * tess_amplitude;
    case 3:
    u = 7. * u;
    v = 7.1 * v;
    float u_f = floor(u);
    float v_f = floor(v);
    float u_c = ceil(u);
    float v_c = ceil(v);
    float r_ff = fract(sin(dot(vec2(u_f,v_f), vec2(12.9898, 78.233))) * 43758.5453) * tess_amplitude;
    float r_fc = fract(sin(dot(vec2(u_f,v_c), vec2(12.9898, 78.233))) * 43758.5453) * tess_amplitude;
    float r_cf = fract(sin(dot(vec2(u_c,v_f), vec2(12.9898, 78.233))) * 43758.5453) * tess_amplitude;
    float r_cc = fract(sin(dot(vec2(u_c,v_c), vec2(12.9898, 78.233))) * 43758.5453) * tess_amplitude;
    float a = mix(r_ff, r_cf, mod(u, 1.));
    float b = mix(r_fc, r_cc, mod(u, 1.));
    return mix(a, b, mod(v, 1.));
  }

  return fract(sin(dot(vec2(u,v), vec2(12.9898, 78.233))) * 43758.5453) * tess_amplitude;
  //return tess_amplitude * sin(2 * M_PI * freq * u) * sin(2 * M_PI * freq * v);
}

vec3 baseSurfacePosition(float u, float v, mat4 Gx, mat4 Gy, mat4 Gz) {
  // Mt * U or MT * V yields B-splines

  // Compute position
  vec4 U = vec4(u*u*u, u*u, u, 1);
  vec4 V = vec4(v*v*v, v*v, v, 1);

  float sX = dot(bicubicMt * U, Gx * bicubicMt * V);
  float sY = dot(bicubicMt * U, Gy * bicubicMt * V);
  float sZ = dot(bicubicMt * U, Gz * bicubicMt * V);

  vec3 s = vec3(sX, sY, sZ);
  return s;
}

vec3 baseSurfaceNormal(float u, float v, mat4 Gx, mat4 Gy, mat4 Gz) {
  // Compute normal using derivatives wrt u and v.
  vec4 U = vec4(u*u*u, u*u, u, 1);
  vec4 V = vec4(v*v*v, v*v, v, 1);

  vec4 dU = vec4(3*u*u, 2*u, 1, 0);
  vec4 dV = vec4(3*v*v, 2*v, 1, 0); 

  float tanUX = dot(bicubicMt * dU, Gx * bicubicMt * V);
  float tanUY = dot(bicubicMt * dU, Gy * bicubicMt * V);
  float tanUZ = dot(bicubicMt * dU, Gz * bicubicMt * V);
  float tanVX = dot(bicubicMt * U, Gx * bicubicMt * dV);
  float tanVY = dot(bicubicMt * U, Gy * bicubicMt * dV);
  float tanVZ = dot(bicubicMt * U, Gz * bicubicMt * dV);

  // Cross product of tangent lines yields normal
  vec3 sU = vec3(tanUX, tanUY, tanUZ);
  vec3 sV = vec3(tanVX, tanVY, tanVZ);

  vec3 normalS = normalize(cross(sU, sV));

  return normalS;
}

float offset(float u, float v, float uC, float vC) {
  // For now consider uniform tessellation levels.
  vec3 U = vec3(u*u, u, 1);
  vec3 V = vec3(v*v, v, 1);

  // vec3 U = vec3(u*u, u, 1);
  // vec3 V = vec3(v*v, v, 1);

  float r = 1 / innerTessLevel;

  mat3 coefficients = mat3(coeff(uC - r, vC - r), coeff(uC, vC - r), coeff(uC + r, vC - r),
                          coeff(uC - r, vC), coeff(uC, vC), coeff(uC + r, vC),
                          coeff(uC - r, vC + r), coeff(uC, vC + r), coeff(uC + r, vC + r));

  float D = dot(biquadraticMt * U, coefficients * biquadraticMt * V);
  return D;
}

void main() {
  // Abstract patch coordinates
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  float uhat = subpatchTransform(u); // Maps to [0,1]
  float vhat = subpatchTransform(v);

  float r = 1 / innerTessLevel;

  float uC = u + r * (0.5 - uhat); 
  float vC = v + r * (0.5 - vhat);  

  // Use three control point matrices, one for each spatial dimension.
  mat4 Gx = mat4(vertcoords_tc[0].x, vertcoords_tc[1].x, vertcoords_tc[2].x, vertcoords_tc[3].x,
              vertcoords_tc[4].x, vertcoords_tc[5].x, vertcoords_tc[6].x, vertcoords_tc[7].x,
              vertcoords_tc[8].x, vertcoords_tc[9].x, vertcoords_tc[10].x, vertcoords_tc[11].x,
              vertcoords_tc[12].x, vertcoords_tc[13].x, vertcoords_tc[14].x, vertcoords_tc[15].x);

  mat4 Gy = mat4(vertcoords_tc[0].y, vertcoords_tc[1].y, vertcoords_tc[2].y, vertcoords_tc[3].y,
              vertcoords_tc[4].y, vertcoords_tc[5].y, vertcoords_tc[6].y, vertcoords_tc[7].y,
              vertcoords_tc[8].y, vertcoords_tc[9].y, vertcoords_tc[10].y, vertcoords_tc[11].y,
              vertcoords_tc[12].y, vertcoords_tc[13].y, vertcoords_tc[14].y, vertcoords_tc[15].y);

  mat4 Gz = mat4(vertcoords_tc[0].z, vertcoords_tc[1].z, vertcoords_tc[2].z, vertcoords_tc[3].z,
              vertcoords_tc[4].z, vertcoords_tc[5].z, vertcoords_tc[6].z, vertcoords_tc[7].z,
              vertcoords_tc[8].z, vertcoords_tc[9].z, vertcoords_tc[10].z, vertcoords_tc[11].z,
              vertcoords_tc[12].z, vertcoords_tc[13].z, vertcoords_tc[14].z, vertcoords_tc[15].z);


  vec3 s = baseSurfacePosition(u, v, Gx, Gy, Gz);
  vec3 Ns = baseSurfaceNormal(u, v, Gx, Gy, Gz);
  float D = offset(uhat, vhat, uC, vC);
  // float D = coeff(u, v);

  vec3 f = s + Ns * D;

  vec3 U = vec3(uhat*uhat, uhat, 1);
  vec3 V = vec3(vhat*vhat, vhat, 1);

  vec3 dU = vec3(2*uhat, 1, 0);
  vec3 dV = vec3(2*vhat, 1, 0);

  mat3 coefficients = mat3(coeff(uC - r, vC - r), coeff(uC, vC - r), coeff(uC + r, vC - r),
                          coeff(uC - r, vC), coeff(uC, vC), coeff(uC + r, vC),
                          coeff(uC - r, vC + r), coeff(uC, vC + r), coeff(uC + r, vC + r));

  float dDdu = innerTessLevel * dot(biquadraticMt * dU, coefficients * biquadraticMt * V);
  float dDdv = innerTessLevel * dot(biquadraticMt * U, coefficients * biquadraticMt * dV);

  vec4 U4 = vec4(u*u*u, u*u, u, 1);
  vec4 V4 = vec4(v*v*v, v*v, v, 1);

  vec4 dU4 = vec4(3*u*u, 2*u, 1, 0);
  vec4 dV4 = vec4(3*v*v, 2*v, 1, 0);

  float tanUX = dot(bicubicMt * dU4, Gx * bicubicMt * V4);
  float tanUY = dot(bicubicMt * dU4, Gy * bicubicMt * V4);
  float tanUZ = dot(bicubicMt * dU4, Gz * bicubicMt * V4);
  float tanVX = dot(bicubicMt * U4, Gx * bicubicMt * dV4);
  float tanVY = dot(bicubicMt * U4, Gy * bicubicMt * dV4);
  float tanVZ = dot(bicubicMt * U4, Gz * bicubicMt * dV4);

  // Cross product of tangent lines yields normal
  vec3 dsdu = vec3(tanUX, tanUY, tanUZ);
  vec3 dsdv = vec3(tanVX, tanVY, tanVZ);


  vec4 dUU4 = vec4(6*u, 2, 0, 0);
  vec4 dVV4 = vec4(6*v, 2, 0, 0);

  float tanUUX = dot(bicubicMt * dUU4, Gx * bicubicMt * V4);
  float tanUUY = dot(bicubicMt * dUU4, Gy * bicubicMt * V4);
  float tanUUZ = dot(bicubicMt * dUU4, Gz * bicubicMt * V4);

  vec3 dsduu = vec3(tanUUX, tanUUY, tanUUZ);

  float tanVVX = dot(bicubicMt * U4, Gx * bicubicMt * dVV4);
  float tanVVY = dot(bicubicMt * U4, Gy * bicubicMt * dVV4);
  float tanVVZ = dot(bicubicMt * U4, Gz * bicubicMt * dVV4);

  vec3 dsdvv = vec3(tanVVX, tanVVY, tanVVZ);

  float tanUVX = dot(bicubicMt * dU4, Gx * bicubicMt * dV4);
  float tanUVY = dot(bicubicMt * dU4, Gy * bicubicMt * dV4);
  float tanUVZ = dot(bicubicMt * dU4, Gz * bicubicMt * dV4);

  vec3 dsduv = vec3(tanUVX, tanUVY, tanUVZ);


  float lenDsdu = length(dsdu);
  float lenDsdv = length(dsdv);

  float Ec = lenDsdu * lenDsdu;
  float Fc = dot(dsdu, dsdv);
  float Gc = lenDsdv * lenDsdv;

  float ec = dot(Ns, dsduu);
  float fc = dot(Ns, dsduv);
  float gc = dot(Ns, dsdvv);

  vec3 dNsnndu = dsdu * (fc*Fc - ec*Gc) / (Ec*Gc - Fc*Fc) + dsdv * (ec*Fc - fc*Ec) / (Ec*Gc- Fc*Fc);
  vec3 dNsnndv = dsdv * (fc*Fc - ec*Gc) / (Ec*Gc - Fc*Fc) + dsdu * (ec*Fc - fc*Ec) / (Ec*Gc - Fc*Fc);

  float NsLength = length(cross(dsdu, dsdv));

  vec3 dNsdu = dNsnndu - Ns * (dot(dNsnndu, Ns)) / NsLength;
  vec3 dNsdv = dNsnndv - Ns * (dot(dNsnndv, Ns)) / NsLength;

  vertU = u;
  vertV = v;

  vertbasesurfacedu_te = dsdu;
  vertbasesurfacedv_te = dsdv;

  vertbasenormal = Ns;

  vertdisplacement = D;
  vertbasenormaldu = dNsdu;
  vertbasenormaldv = dNsdv;

  tileSize = innerTessLevel;

  // Approximate shading
  vec3 dfdu = dsdu + Ns * dDdu;
  vec3 dfdv = dsdv + Ns * dDdv;

  vec3 normalF = normalize(cross(dfdu, dfdv));

  // Multiply with matrices to do coordinate transformations
  gl_Position = projectionmatrix * modelviewmatrix * vec4(f, 1.0);
  vertcoords_te = vec3(modelviewmatrix * vec4(f, 1.0));
  vertnormals_te = normalize(normalmatrix * normalF);
}
