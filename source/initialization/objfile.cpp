#include "objfile.h"

#include <QDebug>
#include <QFile>
#include <QMatrix4x4>

#include "util/util.h"

#define DESIRED_SCALE 2.0

/**
 * @brief OBJFile::OBJFile Reads information from the provided .obj file and
 * stores it in this class.
 * @param fileName The path of the .obj file
 */
OBJFile::OBJFile(const QString &fileName) {
  qDebug() << ":: Loading" << fileName;
  QFile newModel(fileName);

  if (newModel.open(QIODevice::ReadOnly)) {
    QTextStream fileContents(&newModel);

    while (!fileContents.atEnd()) {
      QString currentLine = fileContents.readLine();
      QStringList values = currentLine.split(" ");

      const QString descriptor = values[0];
      if (descriptor == "v") {
        handleVertex(values);
      } else if (descriptor == "vt") {
        handleVertexTexCoords(values);
      } else if (descriptor == "vn") {
        handleVertexNormal(values);
      } else if (descriptor == "f") {
        handleFace(values);
      } else {
        qDebug() << " * Line contents ignored," << currentLine;
      }
    }
    newModel.close();
    normalizeMesh(DESIRED_SCALE);
    loadSuccess = true;
  } else {
    loadSuccess = false;
  }
}

/**
 * @brief OBJFile::~OBJFile Deconstructor.
 */
OBJFile::~OBJFile() {}

/**
 * @brief OBJFile::handleVertex Handles vertex coordinate data. Invoked when the
 * line starts with "v".
 * @param values Line contents split by whitespace.
 */
void OBJFile::handleVertex(const QStringList &values) {
  // Only x, y and z. If there's a w value (homogenous coordinates),
  // ignore it.
  vertexCoords.append(
      QVector3D(values[1].toFloat(), values[2].toFloat(), values[3].toFloat()));
}

/**
 * @brief OBJFile::handleVertexTexCoords Handles vertex texture data. Invoked
 * when the line starts with "vt".
 * @param values Line contents split by whitespace.
 */
void OBJFile::handleVertexTexCoords(const QStringList &values) {
  // Only u and v. If there's a w value (barycentric coordinates), ignore
  // it, it can be retrieved from 1-u-v.
  textureCoords.append(QVector2D(values[1].toFloat(), values[2].toFloat()));
}

/**
 * @brief OBJFile::handleVertexNormal Handles vertex normal data. Invoked
 * when the line starts with "vn".
 * @param values Line contents split by whitespace.
 */
void OBJFile::handleVertexNormal(const QStringList &values) {
  vertexNormals.append(
      QVector3D(values[1].toFloat(), values[2].toFloat(), values[3].toFloat()));
}

/**
 * @brief OBJFile::handleFace Handles face index data. Invoked
 * when the line starts with "f".
 * @param values Line contents split by whitespace.
 */
void OBJFile::handleFace(const QStringList &values) {
  QVector<int> faceCoordsIndices;
  QVector<int> faceTexIndices;
  QVector<int> faceNormalIndices;
  for (int k = 1; k < values.size(); k++) {
    QStringList indices = values[k].split("/");

    // Note -1, OBJ starts indexing from 1.
    faceCoordsIndices.append(indices[0].toInt() - 1);

    if (indices.size() > 1) {
      if (!indices[1].isEmpty()) {
        faceTexIndices.append(indices[1].toInt() - 1);
      }

      if (indices.size() > 2) {
        if (!indices[2].isEmpty()) {
          faceNormalIndices.append(indices[2].toInt() - 1);
        }
      }
    }
  }
  faceCoordInd.append(faceCoordsIndices);
  faceTexIndices.append(faceTexIndices);
  faceNormalIndices.append(faceNormalIndices);
  faceValences.append(values.size() - 1);
}

/**
 * @brief OBJFile::loadedSuccessfully Checks whether the model was loaded from
 * the .obj successfully.
 * @return True if the load was successful, false otherwise.
 */
bool OBJFile::loadedSuccessfully() const { return loadSuccess; }

/**
 * @brief OBJFile::normalizeMesh Scales the information in the obj file in such
 * a way that the mesh fits inside a bounding box of desiredScale.
 * @param desiredScale The desired scale.
 */
void OBJFile::normalizeMesh(float desiredScale) {
  float scale = calcBoundingBoxScale(vertexCoords, desiredScale);
  QMatrix4x4 transformMatrix;
  transformMatrix.setToIdentity();
  transformMatrix.scale(scale);
  for (int i = 0; i < vertexCoords.size(); ++i) {
    QVector3D coord =
        QVector3D(transformMatrix * QVector4D(vertexCoords[i], 1));
    vertexCoords[i] = coord;
  }
}
