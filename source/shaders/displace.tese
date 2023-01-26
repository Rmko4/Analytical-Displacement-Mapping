#version 410

#define M_PI 3.1415926538

layout(quads, fractional_even_spacing, ccw) in;

// layout qualified in and out vars
layout(location = 0) in vec3[] vertcoords_tc;
layout(location = 1) in vec3[] vertnormals_tc;

layout(location = 0) out vec3 vertcoords_te;
layout(location = 1) out vec3 vertnormals_te;

// Out vars
out vec3 vertbasesurfacedu;
out vec3 vertbasesurfacedv;
out vec3 vertbasenormal;

out float vertU;
out float vertV;

out float vertdisplacement;
out vec3 vertbasenormaldu;
out vec3 vertbasenormaldv;

// Uniforms
uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

uniform float tileSize;

uniform float tess_amplitude;

uniform int displacement_mode;
uniform int shading_mode;

// Constants
const float freq = .5F;

const mat4 cubicM = mat4(-1, 3, -3, 1,
                            3, -6, 3, 0,
                            -3, 0, 3, 0,
                            1, 4, 1, 0) / 6;

const mat3 quadratricM = mat3(1, -2,  1,
                                -2,  2,  0,
                                 1,  1,  0) / 2;

// Defined in procedural.glsl
mat3 biquadraticCoeff(float u, float v, float r);

// Transforms abstract patch coordinate to coordinate within biquadratic subpatch
float subpatchTransform(float t) {
  return fract(tileSize * t - 0.5);
}

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
  // ------------------------- Coordinates --------------------------
  
  // Abstract patch coordinates
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  // These are the coordinates of the 3x3 subpatch for displacement
  float uhat = subpatchTransform(u); // Maps to [0,1]
  float vhat = subpatchTransform(v);

  float r = 1 / tileSize;

  // These are the center coordinates of the 3x3 subpatch in the main (u,v) domain.
  float uC = u + r * (0.5 - uhat);
  float vC = v + r * (0.5 - vhat);

  // ------------------------ Bicubic patch -------------------------

  // The cubic basis functions
  vec4 B3u = cubicM * vec4(u*u*u, u*u, u, 1);
  vec4 B3v = cubicM * vec4(v*v*v, v*v, v, 1);

  // The partials of cubic basis functions
  vec4 dB3du = cubicM * vec4(3*u*u, 2*u, 1, 0);
  vec4 dB3dv = cubicM * vec4(3*v*v, 2*v, 1, 0);

  // Base surface s
  vec3 s = tensorAccumulatePatch(B3u, B3v);

  // Partials of base surface s
  vec3 dsdu = tensorAccumulatePatch(dB3du, B3v);
  vec3 dsdv = tensorAccumulatePatch(B3u, dB3dv);

  // Normal of base surface s
  vec3 Ns = normalize(cross(dsdu, dsdv));

  // ---------------------- Biquadratic patch -----------------------

  // The quadratic basis functions
  vec3 B2u = quadratricM * vec3(u*u, u, 1);
  vec3 B2v = quadratricM * vec3(v*v, v, 1);
  
  // The partials of quadratic basis functions
  vec3 dB2du = quadratricM * vec3(2*uhat, 1, 0);
  vec3 dB2dv = quadratricM * vec3(2*vhat, 1, 0);

  // Biquadratic coefficients grid
  mat3 coefficients = biquadraticCoeff(uC, vC, r);

  // Displacement D
  float D = dot(B2u, coefficients * B2v);

  // Partials of displacement D
  float dDdu = tileSize * dot(dB2du, coefficients * B2v);
  float dDdv = tileSize * dot(B2u, coefficients * dB2dv);

  // ---------------------- Displaced surface -----------------------

  // The displaced vertex position f
  vec3 f = s + Ns * D;

  // Partials of displaced surface f
  vec3 dfdu = dsdu + Ns * dDdu;
  vec3 dfdv = dsdv + Ns * dDdv;

  vec3 normalF = normalize(cross(dfdu, dfdv));

  // ------------------------- True shading -------------------------

  if (shading_mode == 2) {
    // The second order partials of cubic basis functions
    vec4 dB3duu = cubicM * vec4(6*u, 2, 0, 0);
    vec4 dB3dvv = cubicM * vec4(6*v, 2, 0, 0);

    // The second order (mixed) partials of base surface s
    vec3 dsduu = tensorAccumulatePatch(dB3duu, B3v);
    vec3 dsdvv = tensorAccumulatePatch(B3u, dB3dvv);
    vec3 dsduv = tensorAccumulatePatch(dB3du, dB3dv);

    float lenDsdu = length(dsdu);
    float lenDsdv = length(dsdv);

    // Coefficients of first fundamental form
    float Ec = lenDsdu * lenDsdu;
    float Fc = dot(dsdu, dsdv);
    float Gc = lenDsdv * lenDsdv;

    // Coefficients of second fundamental form
    float ec = dot(Ns, dsduu);
    float fc = dot(Ns, dsduv);
    float gc = dot(Ns, dsdvv);

    // Partials of non-normalized normals of base surface s 
    vec3 dNsnndu = dsdu * (fc*Fc - ec*Gc) / (Ec*Gc - Fc*Fc) + dsdv * (ec*Fc - fc*Ec) / (Ec*Gc- Fc*Fc);
    vec3 dNsnndv = dsdv * (fc*Fc - ec*Gc) / (Ec*Gc - Fc*Fc) + dsdu * (ec*Fc - fc*Ec) / (Ec*Gc - Fc*Fc);

    float NsLength = length(cross(dsdu, dsdv));

    // Partials of normals of base surface s 
    vec3 dNsdu = dNsnndu - Ns * (dot(dNsnndu, Ns)) / NsLength;
    vec3 dNsdv = dNsnndv - Ns * (dot(dNsnndv, Ns)) / NsLength;

    vertbasenormaldu = dNsdu;
    vertbasenormaldv = dNsdv;
  }

  // ------------------------- Output vars --------------------------

  // Multiply with matrices to do coordinate transformations
  gl_Position = projectionmatrix * modelviewmatrix * vec4(f, 1.0);
  vertcoords_te = vec3(modelviewmatrix * vec4(f, 1.0));
  vertnormals_te = normalize(normalmatrix * normalF);

  vertU = u;
  vertV = v;

  vertbasesurfacedu = dsdu;
  vertbasesurfacedv = dsdv;

  vertbasenormal = Ns;
  vertdisplacement = D;
}
