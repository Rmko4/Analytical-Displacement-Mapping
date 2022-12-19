#ifndef OBJFILE_H
#define OBJFILE_H

#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>

/**
 * @brief The OBJFile class is used for storing info from the .obj files.
 */
class OBJFile {
 public:
  OBJFile(const QString& fileName);
  ~OBJFile();

  bool loadedSuccessfully() const;
  void normalizeMesh(float desiredScale);

 private:
  void handleVertex(const QStringList& values);
  void handleVertexTexCoords(const QStringList& values);
  void handleVertexNormal(const QStringList& values);
  void handleFace(const QStringList& values);

  QVector<QVector3D> vertexCoords;
  QVector<QVector2D> textureCoords;
  QVector<QVector3D> vertexNormals;
  QVector<int> faceValences;
  QVector<QVector<int>> faceCoordInd;
  QVector<QVector<int>> faceTexInd;
  QVector<QVector<int>> faceNormalInd;

  bool loadSuccess;

  friend class MeshInitializer;
};

#endif  // OBJFILE_H
