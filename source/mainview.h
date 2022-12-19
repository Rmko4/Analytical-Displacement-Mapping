#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

#include "mesh/mesh.h"
#include "renderers/meshrenderer.h"
#include "renderers/tessrenderer.h"

/**
 * @brief The MainView class represents the main view of the UI. It handles and
 * orchestrates the different renderers.
 */
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {
  Q_OBJECT

public:
  MainView(QWidget *Parent = nullptr);
  ~MainView() override;

  void updateMatrices();
  void updateUniforms();
  void updateBuffers(Mesh &currentMesh);

protected:
  void initializeGL() override;
  void resizeGL(int newWidth, int newHeight) override;
  void paintGL() override;

  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private:
  QVector2D toNormalizedScreenCoordinates(float x, float y);

  QOpenGLDebugLogger debugLogger;

  // for mouse interactions:
  float scale;
  QVector3D oldVec;
  QQuaternion rotationQuaternion;
  bool dragging;

  MeshRenderer meshRenderer;
  TessellationRenderer tessellationRenderer;

  Settings settings;

  // we make mainwindow a friend so it can access settings
  friend class MainWindow;
private slots:
  void onMessageLogged(QOpenGLDebugMessage Message);
};

#endif // MAINVIEW_H
