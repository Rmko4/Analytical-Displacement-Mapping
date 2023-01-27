# Analytical-Displacement-Mapping
### Implementation of analytical displacement mapping using hardware tessellation based on [Nießner & Loop (2013)](https://dl.acm.org/doi/10.1145/2487228.2487234)
![Analytical displacement mapping overview](images/overview-scaled.png)

## Code
The components of the application related to analytical displacement mapping:
- [tessrenderer.cpp](source/renderers/tessrenderer.cpp) - The main renderer class for tessellation
- [procedural.glsl](source/shaders/procedural.glsl) - Procedural generation of displacement coefficients
- [displace.vert](source/shaders/displace.vert) - *Vertex Shader* for displacement mapping
- [displace.tesc](source/shaders/displace.tesc) - *Tessellation Control Shader* for displacement mapping
- [displace.tese](source/shaders/displace.tese) - *Tessellation Evaluation Shader* for displacement mapping
- [displace.frag](source/shaders/displace.frag) - *Fragment Shader* for displacement mapping

## Overview of main features

### Procedural textures:
- Bubblewrap
![Bubblewrap](images/bubblewrap-displacement.png)
- Pinhead
![Pinhead](images/pinhead-displacement.png)
- Chocolate bar
![Chocolate bar](images/chocolatebar-displacement.png)
- Random
![Random](images/random-displacement.png)
### Normal types:
- Interpolated normals
![Interpolation normals](images/interpolated-normals.png)
- Approximate normals
![Approximate normals](images/approximate-normals.png)
- True normals
![True normals](images/true-normals.png)

### Shading:
- Phong shading (see examples above)
- Normal shading (Visualization for all types of normals)
![Normal shading](images/normals-shading.png)
- Approximation Error (Error of approximate errors compared to true normals)
![Alt text](images/approximation-error.png)