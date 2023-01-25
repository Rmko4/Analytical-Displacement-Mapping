#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMatrix4x4>

#include "shadertypes.h"

/**
 * Struct that contains all the settings of the program. Initialised with a
 * number of default values.
 */
typedef struct Settings {
  bool modelLoaded = false;
  bool wireframeMode = true;
  bool tesselationMode = false;
  bool showCpuMesh = true;

  int subdivSteps = 0;

  float FoV = 80;
  float dispRatio = 16.0f / 9.0f;
  float rotAngle = 0.0f;

  float innerTessellationLevel = 4.F;
  float outerTessellationLevel = 4.F;

  bool dynamicLoD = false;
  float tessDetail = 10.F;

  // Displacement stuff:
  float amplitude = 0.2;
  int displacement_mode = 0;

  // Shading mode:
  int shading_mode = 0; // 0 is phong. 1 is normals
  int normal_mode = 0; // 0 is true normals; 1 is approx normals; 2 is interpolated normals

  bool uniformUpdateRequired = true;

  ShaderType currentMeshShader = ShaderType::PHONG;
  ShaderType currentTessellationShader = ShaderType::BICUBIC;

  QMatrix4x4 modelViewMatrix, projectionMatrix;
  QMatrix3x3 normalMatrix;
} Settings;

#endif // SETTINGS_H
