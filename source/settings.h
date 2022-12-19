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
  bool limitProjectionMode = false;

  int subdivSteps = 0;

  float FoV = 80;
  float dispRatio = 16.0f / 9.0f;
  float rotAngle = 0.0f;

  float innerTessellationLevel = 1.F;
  float outerTessellationLevel = 1.F;

  bool uniformUpdateRequired = true;

  ShaderType currentMeshShader = ShaderType::PHONG;
  ShaderType currentTessellationShader = ShaderType::BILINEAR;

  QMatrix4x4 modelViewMatrix, projectionMatrix;
  QMatrix3x3 normalMatrix;
} Settings;

#endif // SETTINGS_H
