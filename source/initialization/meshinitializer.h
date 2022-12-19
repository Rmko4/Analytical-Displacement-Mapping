#ifndef MESH_INITIALIZER_H
#define MESH_INITIALIZER_H

#include "../mesh/mesh.h"
#include "objfile.h"

/**
 * @brief The MeshInitializer class initializes half-edge meshes from OBJFiles.
 */
class MeshInitializer {
 public:
  MeshInitializer();
  Mesh constructHalfEdgeMesh(const OBJFile& loadedOBJFile);

 private:
  void initGeometry(Mesh& mesh, int numVertices,
                    const QVector<QVector3D>& vertexCoords);
  void initTopology(Mesh& mesh, int numFaces,
                     const QVector<QVector<int>>& faceCoordInd);
  void addHalfEdge(Mesh& mesh, int h, Face* face,
                   const QVector<int>& faceIndices, int i);
  void setTwins(Mesh& mesh, int h, int vertIdx1, int vertIdx2);

  QList<QPair<int, int>> edgeList;
  QVector<int> edgeIndices;
};

#endif  // MESH_INITIALIZER_H
