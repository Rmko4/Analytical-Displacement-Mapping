#version 410
// Fragment shader

#define M_PI 3.1415926538

layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;

layout(location = 2) in vec3 vertbasesurfacedu_fs;
layout(location = 3) in vec3 vertbasesurfacedv_fs;
layout(location = 4) in vec3 vertbasenormal;

in float vertU;
in float vertV;

in float tileSize;

in float vertdisplacement;
in vec3 vertbasenormaldu;
in vec3 vertbasenormaldv;


out vec4 fColor;


uniform mat3 normalmatrix;

uniform float tess_amplitude;
uniform int displacement_mode;

const float freq = .5F;

// Defined in shading.glsl
vec3 phongShading(vec3 matCol, vec3 coords, vec3 normal);

float subpatchTransform(float t) {
  return fract(tileSize * t - 0.5);
}

const mat3 biquadraticMt = mat3(1, -2,  1,
                                -2,  2,  0,
                                 1,  1,  0) / 2;


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

void main() {
  float u = subpatchTransform(vertU); // Maps to [0,1]
  float v = subpatchTransform(vertV);

  vec3 Ns = vertbasenormal;

  vec3 dsdu = vertbasesurfacedu_fs;
  vec3 dsdv = vertbasesurfacedv_fs;

  float D = vertdisplacement;

  vec3 dNsdu = vertbasenormaldu;
  vec3 dNsdv = vertbasenormaldv;


  float r = 1 / tileSize;

// Yields the center coordinates [0,1], such that it is on k*r for any positive integer k.
  // float uC = vertU;
  // float vC = vertV;
  
  float uC = vertU + r * (0.5 - u); 
  float vC = vertV + r * (0.5 - v);  
  
  mat3 coefficients = mat3(coeff(uC - r, vC - r), coeff(uC, vC - r), coeff(uC + r, vC - r),
                          coeff(uC - r, vC), coeff(uC, vC), coeff(uC + r, vC),
                          coeff(uC - r, vC + r), coeff(uC, vC + r), coeff(uC + r, vC + r));

  vec3 U = vec3(u*u, u, 1);
  vec3 V = vec3(v*v, v, 1);

  vec3 dU = vec3(2*u, 1, 0);
  vec3 dV = vec3(2*v, 1, 0);

  // vec3 U = vec3(0.5*0.5, 0.5, 1);
  // vec3 V = vec3(0.5*0.5, 0.5, 1);

  // vec3 dU = vec3(2*0.5, 1, 0);
  // vec3 dV = vec3(2*0.5, 1, 0);


  float dDdu = tileSize * dot(biquadraticMt * dU, coefficients * biquadraticMt * V);
  float dDdv = tileSize * dot(biquadraticMt * U, coefficients * biquadraticMt * dV);

  // Approximate shading
  vec3 dfdu = dsdu + Ns * dDdu + dNsdu * D;
  vec3 dfdv = dsdv + Ns * dDdv + dNsdv * D;

  vec3 normalF = normalize(cross(dfdu, dfdv));
  normalF = normalize(normalmatrix * normalF);

  vec3 normalS = normalize(normalmatrix * Ns);

  // vec3 normalColor = 0.5 * normalize(vertnormal_fs) + vec3(0.5, 0.5, 0.5);
  // fColor = vec4(normalColor, 1.0);
  vec3 matcolour = vec3(0.53, 0.80, 0.87);
  // vec3 col = phongShading(matcolour, vertcoords_fs, vertnormal_fs);

  // vec3 col = vec3(0.2 * dDdu, 0.2 * dDdv, 0);
  vec3 col = phongShading(matcolour, vertcoords_fs, normalF);

  fColor = vec4(col, 1.0);
  // Quite hacky trick, but this makes sure that if the control mesh and the
  // tessellated mesh are very close to each other, the tessellated mesh is
  // rendered on top. This prevents flickering.
  // Feel free to remove if you adjusted the UI to prevent this from happening.
  gl_FragDepth = gl_FragCoord.z + 0.00001;
}
