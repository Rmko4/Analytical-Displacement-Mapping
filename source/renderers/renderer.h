#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>

#include "../settings.h"
#include "../shadertypes.h"

/**
 * @brief The Renderer class represents a generic renderer class. The class is
 * essentially abstract and should only contain functionality that is applicable
 * to every renderer.
 */
class Renderer {
 public:
  Renderer();
  Renderer(QOpenGLFunctions_4_1_Core *functions, Settings *settings);
  virtual ~Renderer();

  virtual void initShaders() = 0;
  virtual void initBuffers() = 0;

  QOpenGLShaderProgram *constructDefaultShader(const QString &name) const;
  void init(QOpenGLFunctions_4_1_Core *f, Settings *s);

 protected:
  QMap<ShaderType, QOpenGLShaderProgram *> shaders;
  QOpenGLFunctions_4_1_Core *gl;
  Settings *settings;
};

#endif  // RENDERER_H
