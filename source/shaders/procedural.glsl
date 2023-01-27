#version 410

#define M_PI 3.1415926538

// Uniforms
uniform int displacement_mode;
uniform float tess_amplitude;

// Constants
const float freq = .5F;

// Procedural generation of displacement coefficients.
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

  // This is more than needed:
  // also making each of the 4 triangle into two mirrored right angle triangles
  if (u > 0.5) 
    u = 1. - u;

  switch(displacement_mode)
  {
    case 0: // 2D sinusoid (Bubblewrap)
      return tess_amplitude * sin(2 * M_PI * freq * u) * sin(2 * M_PI * freq * v);
    case 1: // Pinhead
      if (v > 0.4501) return 2 * tess_amplitude;
      return min(1.0, v * 10.0) * tess_amplitude - tess_amplitude;
    case 2: // Chocolate bar
      return min(1.0, v * 5.0) * tess_amplitude;
    case 3: // Pseudo-random
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
}

// Creates 3x3 grid of coefficients with center (u,v) and step size r
mat3 biquadraticCoeff(float u, float v, float r) {
    return mat3(coeff(u - r, v - r), coeff(u, v - r), coeff(u + r, v - r),
                coeff(u - r, v), coeff(u, v), coeff(u + r, v),
                coeff(u - r, v + r), coeff(u, v + r), coeff(u + r, v + r));
}