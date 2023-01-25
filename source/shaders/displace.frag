#version 410
// Fragment shader

#define M_PI 3.1415926538

// Layout qualified in vars
layout(location = 0) in vec3 vertcoords_fs;
layout(location = 1) in vec3 vertnormal_fs;

// In vars
in vec3 vertbasesurfacedu;
in vec3 vertbasesurfacedv;
in vec3 vertbasenormal;

in float vertU;
in float vertV;

in float vertdisplacement;
in vec3 vertbasenormaldu;
in vec3 vertbasenormaldv;

// Out vars
out vec4 fColor;

// Uniforms
uniform mat3 normalmatrix;

uniform float tileSize;
uniform float tess_amplitude;

uniform int displacement_mode;
uniform int shading_mode;
uniform int normal_mode;

// Constants
const float freq = .5F;
const vec3 matcolour = vec3(0.53, 0.80, 0.87);

// Defined in shading.glsl
vec3 phongShading(vec3 matCol, vec3 coords, vec3 normal);

// Defined in procedural.glsl
mat3 biquadraticCoeff(float u, float v, float r);

float subpatchTransform(float t) {
  return fract(tileSize * t - 0.5);
}

const mat3 biquadraticMt = mat3(1, -2,  1,
                                -2,  2,  0,
                                 1,  1,  0) / 2;



void main() {
  float u = subpatchTransform(vertU); // Maps to [0,1]
  float v = subpatchTransform(vertV);

  vec3 Ns = vertbasenormal;

  vec3 dsdu = vertbasesurfacedu;
  vec3 dsdv = vertbasesurfacedv;

  float D = vertdisplacement;

  vec3 dNsdu = vertbasenormaldu;
  vec3 dNsdv = vertbasenormaldv;

  float r = 1 / tileSize;

// Yields the center coordinates [0,1], such that it is on k*r for any positive integer k.
  float uC = vertU + r * (0.5 - u); 
  float vC = vertV + r * (0.5 - v);  
  
  mat3 coefficients = biquadraticCoeff(uC, vC, r);

  vec3 U = vec3(u*u, u, 1);
  vec3 V = vec3(v*v, v, 1);

  vec3 dU = vec3(2*u, 1, 0);
  vec3 dV = vec3(2*v, 1, 0);


  float dDdu = tileSize * dot(biquadraticMt * dU, coefficients * biquadraticMt * V);
  float dDdv = tileSize * dot(biquadraticMt * U, coefficients * biquadraticMt * dV);

  // Interpolated approximate normals shading
  // vec3 col = phongShading(matcolour, vertcoords_fs, vertnormal_fs);
  
  // Approximate normals shading
  vec3 dfduApprox = dsdu + Ns * dDdu;
  vec3 dfdvvApprox = dsdv + Ns * dDdv;

  vec3 normalFApprox = normalize(cross(dfduApprox, dfdvvApprox));
  normalFApprox = normalize(normalmatrix * normalFApprox);

  // vec3 col = phongShading(matcolour, vertcoords_fs, normalFApprox);


  // True normals shading
  vec3 dfdu = dsdu + Ns * dDdu + dNsdu * D;
  vec3 dfdv = dsdv + Ns * dDdv + dNsdv * D;

  vec3 normalF = normalize(cross(dfdu, dfdv));
  normalF = normalize(normalmatrix * normalF);


  // Not putting all the above calculations inside the if clause because glsl can't do
  // real conditional branching anyway
  vec3 finalNormal = normalF;
  if (normal_mode == 1) {
      finalNormal = normalize(normalFApprox);
  }
  else if (normal_mode == 2) {
      finalNormal = normalize(vertnormal_fs);
  }



  vec3 col = phongShading(matcolour, vertcoords_fs, finalNormal);





  // vec3 normalS = normalize(normalmatrix * Ns);

  // vec3 col = vec3(0.2 * dDdu, 0.2 * dDdv, 0);
  // vec3 col = vec3(vertU, vertV, 0);
  // vec3 col = vec3(dNsdu[0], dNsdu[1], dNsdu[2]);

  // vec4 posC = projectionmatrix * modelviewmatrix * vec4(vertcoords_fs, 1.0);
  // float z = posC.z;
  // z /= posC.w;
  // vec3 col = vec3((1.0 - z)/2, posC.w/10, posC.z);


  // Use any of the three normals
  vec3 normalColor = 0.5 * normalize(finalNormal) + vec3(0.5, 0.5, 0.5);


  // Not putting all the above calculations inside the if clause because glsl can't do
  // real conditional branching anyway
  if (shading_mode == 0) {
    // Phong shading:
    fColor = vec4(col, 1.0);
  }
  else {
    // Normal shading:
    fColor = vec4(normalColor, 1.0);
  }



  // Quite hacky trick, but this makes sure that if the control mesh and the
  // tessellated mesh are very close to each other, the tessellated mesh is
  // rendered on top. This prevents flickering.
  // Feel free to remove if you adjusted the UI to prevent this from happening.
  gl_FragDepth = gl_FragCoord.z + 0.00001;
}
