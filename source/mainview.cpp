#include "mainview.h"

#include <math.h>

#include <QLoggingCategory>
#include <QOpenGLVersionFunctionsFactory>

/**
 * @brief MainView::MainView
 * @param Parent
 */
MainView::MainView(QWidget *Parent) : QOpenGLWidget(Parent), scale(1.0f) {}

/**
 * @brief MainView::~MainView Deconstructs the main view.
 */
MainView::~MainView() {
  debugLogger.stopLogging();
  makeCurrent();
}

/**
 * @brief MainView::initializeGL Initializes the opengl functions and settings,
 * initialises the renderers and sets up the debugger.
 */
void MainView::initializeGL() {
  initializeOpenGLFunctions();
  qDebug() << ":: OpenGL initialized";

  connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger.initialize()) {
    QLoggingCategory::setFilterRules("qt.*=false\n"
                                     "qt.text.font.*=false");
    qDebug() << ":: Logging initialized";
    debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    debugLogger.enableMessages();
  }

  QString glVersion;
  glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  makeCurrent();
  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  // grab the opengl context
  QOpenGLFunctions_4_1_Core *functions =
      QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(
          this->context());

  // initialize renderers here with the current context
  meshRenderer.init(functions, &settings);
  tessellationRenderer.init(functions, &settings);

  updateMatrices();
}

/**
 * @brief MainView::resizeGL Handles window resizing.
 * @param newWidth The new width of the window in pixels.
 * @param newHeight The new height of the window in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  qDebug() << ".. resizeGL";

  settings.dispRatio = float(newWidth) / float(newHeight);

  settings.projectionMatrix.setToIdentity();
  settings.projectionMatrix.perspective(settings.FoV, settings.dispRatio, 0.1f,
                                        40.0f);
  updateMatrices();
}

/**
 * @brief MainView::updateMatrices Updates the matrices used for the model
 * transforms.
 */
void MainView::updateMatrices() {
  settings.modelViewMatrix.setToIdentity();
  settings.modelViewMatrix.translate(QVector3D(0.0, 0.0, -3.0));
  settings.modelViewMatrix.scale(scale);
  settings.modelViewMatrix.rotate(rotationQuaternion);

  settings.normalMatrix = settings.modelViewMatrix.normalMatrix();

  settings.uniformUpdateRequired = true;

  update();
}

/**
 * @brief MainView::updateBuffers Updates the buffers of the renderers.
 * @param mesh The mesh used to update the buffer content with.
 */
void MainView::updateBuffers(Mesh &mesh) {
  mesh.extractAttributes();
  // Only extract/compute the following attributes when the corresponding mode
  // is selected.
  if (settings.currentTessellationShader == ShaderType::BICUBIC or
      settings.currentTessellationShader == ShaderType::DISPLACEMENT) {
    mesh.computeRegularPatchIndices();
  }
  if (settings.limitProjectionMode) {
    mesh.computeLimitVertices();
  }

  meshRenderer.updateBuffers(mesh);
  tessellationRenderer.updateBuffers(mesh);
  update();
}

/**
 * @brief MainView::paintGL Draw call.
 */
void MainView::paintGL() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (settings.wireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if (settings.modelLoaded) {
    if (settings.showCpuMesh) {
      meshRenderer.draw();
    }
    if (settings.tesselationMode) {
      tessellationRenderer.draw();
    }

    if (settings.uniformUpdateRequired) {
      settings.uniformUpdateRequired = false;
    }
  }
}

/**
 * @brief MainView::toNormalizedScreenCoordinates Normalizes the mouse
 * coordinates to screen coordinates.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return A vector containing the normalized x and y screen coordinates.
 */
QVector2D MainView::toNormalizedScreenCoordinates(float x, float y) {
  float xRatio = x / float(width());
  float yRatio = y / float(height());

  // By default, the drawing canvas is the square [-1,1]^2:
  float xScene = (1 - xRatio) * -1 + xRatio * 1;
  // Note that the origin of the canvas is in the top left corner (not the lower
  // left).
  float yScene = yRatio * -1 + (1 - yRatio) * 1;

  return {xScene, yScene};
}

/**
 * @brief MainView::mouseMoveEvent Handles the dragging and rotating of the mesh
 * by looking at the mouse movement.
 * @param event Mouse event.
 */
void MainView::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::LeftButton) {
    QVector2D sPos = toNormalizedScreenCoordinates(event->position().x(),
                                                   event->position().y());
    QVector3D newVec = QVector3D(sPos.x(), sPos.y(), 0.0);

    // project onto sphere
    float sqrZ = 1.0f - QVector3D::dotProduct(newVec, newVec);
    if (sqrZ > 0) {
      newVec.setZ(sqrt(sqrZ));
    } else {
      newVec.normalize();
    }

    QVector3D v2 = newVec.normalized();
    // reset if we are starting a drag
    if (!dragging) {
      dragging = true;
      oldVec = newVec;
      return;
    }

    // calculate axis and angle
    QVector3D v1 = oldVec.normalized();
    QVector3D N = QVector3D::crossProduct(v1, v2).normalized();
    if (N.length() == 0.0f) {
      oldVec = newVec;
      return;
    }
    float angle = 180.0f / M_PI * acos(QVector3D::dotProduct(v1, v2));
    rotationQuaternion =
        QQuaternion::fromAxisAndAngle(N, angle) * rotationQuaternion;
    updateMatrices();

    // for next iteration
    oldVec = newVec;
  } else {
    // to reset drag
    dragging = false;
    oldVec = QVector3D();
  }
}

/**
 * @brief MainView::mousePressEvent Handles presses by the mouse. Currently does
 * nothing except setting focus.
 * @param event Mouse event.
 */
void MainView::mousePressEvent(QMouseEvent *event) { setFocus(); }

/**
 * @brief MainView::wheelEvent Handles zooming of the view.
 * @param event Mouse event.
 */
void MainView::wheelEvent(QWheelEvent *event) {
  // Delta is usually 120
  float phi = 1.0f + (event->angleDelta().y() / 2000.0f);
  scale = fmin(fmax(phi * scale, 0.01f), 100.0f);
  updateMatrices();
}

/**
 * @brief MainView::keyPressEvent Handles keyboard shortcuts. Currently support
 * 'Z' for wireframe mode and 'R' to reset orientation.
 * @param event Mouse event.
 */
void MainView::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case 'Z':
    settings.wireframeMode = !settings.wireframeMode;
    update();
    break;
  case 'R':
    scale = 1.0f;
    rotationQuaternion = QQuaternion();
    updateMatrices();
    update();
    break;
  }
}

/**
 * @brief MainView::onMessageLogged Helper function for logging messages.
 * @param message The message to log.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}
