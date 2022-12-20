#include "tessrenderer.h"

/**
 * @brief TessellationRenderer::TessellationRenderer Creates a new tessellation
 * renderer.
 */
TessellationRenderer::TessellationRenderer() : meshIBOSize(0) {}

/**
 * @brief TessellationRenderer::~TessellationRenderer Deconstructor.
 */
TessellationRenderer::~TessellationRenderer() {
  gl->glDeleteVertexArrays(1, &vao);

  gl->glDeleteBuffers(1, &meshCoordsBO);
  gl->glDeleteBuffers(1, &meshNormalsBO);
  gl->glDeleteBuffers(1, &meshIndexBO);
}

/**
 * @brief TessellationRenderer::initShaders Initializes the shaders used for the
 * Tessellation.
 */
void TessellationRenderer::initShaders() {
  shaders[ShaderType::BILINEAR] = constructTesselationShader("patch");
  shaders[ShaderType::BICUBIC] = constructTesselationShader("bicubic");
  shaders[ShaderType::DISPLACEMENT] = constructTesselationShader("displace");
}

/**
 * @brief TessellationRenderer::constructTesselationShader Constructs a shader
 * consisting of a vertex shader, tessellation control shader, tessellation
 * evaluation shader and a fragment shader. The shaders are assumed to follow
 * the naming convention: <name>.vert, <name.tesc>, <name.tese> and <name>.frag.
 * All of these files have to exist for this function to work successfully.
 * @param name Name of the shader.
 * @return The constructed shader.
 */
QOpenGLShaderProgram *
TessellationRenderer::constructTesselationShader(const QString &name) const {
  QString pathVert = ":/shaders/" + name + ".vert";
  QString pathTesC = ":/shaders/" + name + ".tesc";
  QString pathTesE = ":/shaders/" + name + ".tese";
  QString pathFrag = ":/shaders/" + name + ".frag";
  QString pathShading = ":/shaders/shading.glsl";

  // we use the qt wrapper functions for shader objects
  QOpenGLShaderProgram *shader = new QOpenGLShaderProgram();
  shader->addShaderFromSourceFile(QOpenGLShader::Vertex, pathVert);
  shader->addShaderFromSourceFile(QOpenGLShader::TessellationControl, pathTesC);
  shader->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation,
                                  pathTesE);
  shader->addShaderFromSourceFile(QOpenGLShader::Fragment, pathFrag);
  shader->addShaderFromSourceFile(QOpenGLShader::Fragment, pathShading);
  shader->link();
  return shader;
}

/**
 * @brief TessellationRenderer::initBuffers Initializes the buffers. Uses
 * indexed rendering. The coordinates and normals are passed into the shaders.
 */
void TessellationRenderer::initBuffers() {
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
 * @brief TessellationRenderer::updateBuffers Updates the buffers based on the
 * provided mesh.
 * @param mesh The mesh to update the buffer contents with.
 */
void TessellationRenderer::updateBuffers(Mesh &currentMesh) {
  QVector<QVector3D> &vertexCoords = currentMesh.getVertexCoords();
  QVector<QVector3D> &vertexNormals = currentMesh.getVertexNorms();

  QVector<unsigned int> *meshIndices;
  if (settings->currentTessellationShader == ShaderType::BILINEAR) {
    meshIndices = &currentMesh.getQuadIndices();
  } else { // If current shader is bicubic
    meshIndices = &currentMesh.getRegularPatchIndices();
  }

  gl->glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
  gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertexCoords.size(),
                   vertexCoords.data(), GL_DYNAMIC_DRAW);

  gl->glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
  gl->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * vertexNormals.size(),
                   vertexNormals.data(), GL_DYNAMIC_DRAW);

  gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);
  gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(unsigned int) * meshIndices->size(),
                   meshIndices->data(), GL_DYNAMIC_DRAW);

  meshIBOSize = meshIndices->size();
}

/**
 * @brief TessellationRenderer::updateUniforms Updates the uniforms in the
 * shader.
 */
void TessellationRenderer::updateUniforms() {
  QOpenGLShaderProgram *shader = shaders[settings->currentTessellationShader];
  uniModelViewMatrix = shader->uniformLocation("modelviewmatrix");
  uniProjectionMatrix = shader->uniformLocation("projectionmatrix");
  uniNormalMatrix = shader->uniformLocation("normalmatrix");
  uniInnerTessLevel = shader->uniformLocation("innerTessLevel");
  uniOuterTessLevel = shader->uniformLocation("outerTessLevel");

  gl->glUniformMatrix4fv(uniModelViewMatrix, 1, false,
                         settings->modelViewMatrix.data());
  gl->glUniformMatrix4fv(uniProjectionMatrix, 1, false,
                         settings->projectionMatrix.data());
  gl->glUniformMatrix3fv(uniNormalMatrix, 1, false,
                         settings->normalMatrix.data());
  gl->glUniform1f(uniInnerTessLevel, settings->innerTessellationLevel);
  gl->glUniform1f(uniOuterTessLevel, settings->outerTessellationLevel);
}

/**
 * @brief MeshRenderer::draw Draw call.
 */
void TessellationRenderer::draw() {
  shaders[settings->currentTessellationShader]->bind();

  if (settings->uniformUpdateRequired) {
    updateUniforms();
  }

  gl->glBindVertexArray(vao);

  GLuint patchSize =
      settings->currentTessellationShader == ShaderType::BILINEAR ? 4 : 16;

  gl->glPatchParameteri(GL_PATCH_VERTICES, patchSize);
  gl->glDrawElements(GL_PATCHES, meshIBOSize, GL_UNSIGNED_INT, nullptr);

  gl->glBindVertexArray(0);

  shaders[settings->currentTessellationShader]->release();
}
