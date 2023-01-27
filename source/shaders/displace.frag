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

uniform sampler1D cmap;

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

const mat3 quadratricM = mat3(1, -2,  1,
                                -2,  2,  0,
                                 1,  1,  0) / 2;

void main() {
  vec3 dsdu = vertbasesurfacedu;
  vec3 dsdv = vertbasesurfacedv;
  vec3 Ns = vertbasenormal;

  float dDdu;
  float dDdv;

  // -------------------- Displacement partials ---------------------
  
  // Non-interpolatory case
  if (normal_mode == 0 || normal_mode == 1 || shading_mode == 2) {
    // These are the coordinates of the 3x3 subpatch for displacement
    float u = subpatchTransform(vertU); // Maps to [0,1]
    float v = subpatchTransform(vertV);

    float r = 1 / tileSize;

    // These are the center coordinates of the 3x3 subpatch in the main (u,v) domain.
    float uC = vertU + r * (0.5 - u); 
    float vC = vertV + r * (0.5 - v);  

    // The quadratic basis functions
    vec3 B2u = quadratricM * vec3(u*u, u, 1);
    vec3 B2v = quadratricM * vec3(v*v, v, 1);
    
    // The partials of quadratic basis functions
    vec3 dB2du = quadratricM * vec3(2*u, 1, 0);
    vec3 dB2dv = quadratricM * vec3(2*v, 1, 0);

    // Biquadratic coefficients grid
    mat3 coefficients = biquadraticCoeff(uC, vC, r);

    // Partials of displacement D
    dDdu = tileSize * dot(dB2du, coefficients * B2v);
    dDdv = tileSize * dot(B2u, coefficients * dB2dv);
  }

  // --------------------- Normal computation  ----------------------
  vec3 NfApprox, Nf;
  vec3 finalNormal = vertnormal_fs;

  if (normal_mode == 1 || shading_mode == 2) {
    // Approximate normals shading
    vec3 dfduApprox = dsdu + Ns * dDdu;
    vec3 dfdvvApprox = dsdv + Ns * dDdv;

    NfApprox = normalize(cross(dfduApprox, dfdvvApprox));
    NfApprox = normalize(normalmatrix * NfApprox);

    finalNormal = NfApprox;
  }
  if (normal_mode == 0 || shading_mode == 2) {
    // True normals shading
    vec3 dNsdu = vertbasenormaldu;
    vec3 dNsdv = vertbasenormaldv;
    float D = vertdisplacement;

    vec3 dfdu = dsdu + Ns * dDdu + dNsdu * D;
    vec3 dfdv = dsdv + Ns * dDdv + dNsdv * D;

    Nf = normalize(cross(dfdu, dfdv));
    Nf = normalize(normalmatrix * Nf);
    
    finalNormal = Nf;
  }

  // --------------------------- Shading ----------------------------

  vec3 color;
  if (shading_mode == 0) {
    // Phong shading:
    color = phongShading(matcolour, vertcoords_fs, finalNormal);
  }
  else if (shading_mode == 1) {
    // Normal shading:
    color = 0.5 * normalize(finalNormal) + vec3(0.5, 0.5, 0.5);
  } else {
    float error = acos(dot(NfApprox, Nf)) / M_PI;
    color = vec3(texture(cmap, error));
  }

  fColor = vec4(color, 1.0);

  // Quite hacky trick, but this makes sure that if the control mesh and the
  // tessellated mesh are very close to each other, the tessellated mesh is
  // rendered on top. This prevents flickering.
  // Feel free to remove if you adjusted the UI to prevent this from happening.
  gl_FragDepth = gl_FragCoord.z + 0.00001;
}
