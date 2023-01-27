#include "meshrenderer.h"

/**
 * @brief MeshRenderer::MeshRenderer Creates a new mesh renderer.
 */
MeshRenderer::MeshRenderer() : meshIBOSize(0) {}

/**
 * @brief MeshRenderer::~MeshRenderer Deconstructor.
 */
MeshRenderer::~MeshRenderer() {
  gl->glDeleteVertexArrays(1, &vao);

  gl->glDeleteBuffers(1, &meshCoordsBO);
  gl->glDeleteBuffers(1, &meshNormalsBO);
  gl->glDeleteBuffers(1, &meshIndexBO);
}

/**
 * @brief MeshRenderer::initShaders Initializes the shaders used to shade a
 * mesh.
 */
void MeshRenderer::initShaders() {
  shaders.insert(ShaderType::PHONG, constructDefaultShader("phong"));
}

/**
 * @brief MeshRenderer::initBuffers Initializes the buffers. Uses indexed
 * rendering. The coordinates and normals are passed into the shaders.
 */
void MeshRenderer::initBuffers() {
  gl->glGenVertexArrays(1, &vao);
  gl->glBindVertexArray(vao);

  gl->glGenBuffers(1, &meshCoordsBO);
  gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
  gl->glEnableVertexAttribArray(0);
  gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  gl->glGenBuffers(1, &meshNormalsBO);
  gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
  gl->glEnableVertexAttribArray(1);
  gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  gl->glGenBuffers(1, &meshIndexBO);
  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);

  gl->glBindVertexArray(0);
}

/**
 * @brief MeshRenderer::updateBuffers Updates the buffers based on the provided
 * mesh.
 * @param mesh The mesh to update the buffer contents with.
 */
void MeshRenderer::updateBuffers(Mesh &mesh) {
  QVector<QVector3D> &vertexCoords = mesh.getVertexCoords();
  QVector<QVector3D> &vertexNormals = mesh.getVertexNorms();
  QVector<unsigned int> &polyIndices = mesh.getPolyIndices();

  gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
  gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertexCoords.size(),
                   vertexCoords.data(), GL_STATIC_DRAW);

  gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
  gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertexNormals.size(),
                   vertexNormals.data(), GL_STATIC_DRAW);

  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);
  gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(unsigned int) * polyIndices.size(),
                   polyIndices.data(), GL_STATIC_DRAW);

  meshIBOSize = polyIndices.size();
}

/**
 * @brief MeshRenderer::updateUniforms Updates the uniforms in the shader.
 */
void MeshRenderer::updateUniforms() {
  QOpenGLShaderProgram *shader = shaders[settings->currentMeshShader];

  uniModelViewMatrix = shader->uniformLocation("modelviewmatrix");
  uniProjectionMatrix = shader->uniformLocation("projectionmatrix");
  uniNormalMatrix = shader->uniformLocation("normalmatrix");

  uniShadingMode = shader->uniformLocation("shading_mode");

  gl->glUniformMatrix4fv(uniModelViewMatrix, 1, false,
                         settings->modelViewMatrix.data());
  gl->glUniformMatrix4fv(uniProjectionMatrix, 1, false,
                         settings->projectionMatrix.data());
  gl->glUniformMatrix3fv(uniNormalMatrix, 1, false,
                         settings->normalMatrix.data());

  int shading_mode =
      settings->shading_mode == 1 &&
              settings->currentTessellationShader == ShaderType::DISPLACEMENT &&
              settings->tesselationMode == true
          ? 1
          : 0;

  gl->glUniform1i(uniShadingMode, shading_mode);
}

/**
 * @brief MeshRenderer::draw Draw call.
 */
void MeshRenderer::draw() {
  shaders[settings->currentMeshShader]->bind();

  if (settings->uniformUpdateRequired) {
    updateUniforms();
  }
  // enable primitive restart to allow for drawing faces of arbitrary valence
  gl->glEnable(GL_PRIMITIVE_RESTART);
  gl->glPrimitiveRestartIndex(INT_MAX);

  gl->glBindVertexArray(vao);

  if (settings->wireframeMode) {
    gl->glDrawElements(GL_LINE_LOOP, meshIBOSize, GL_UNSIGNED_INT, nullptr);
  } else {
    gl->glDrawElements(GL_TRIANGLE_FAN, meshIBOSize, GL_UNSIGNED_INT, nullptr);
  }

  gl->glBindVertexArray(0);

  shaders[settings->currentMeshShader]->release();

  // disable it again as you might want to draw something else at some point
  gl->glDisable(GL_PRIMITIVE_RESTART);
}
