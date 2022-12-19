#include "meshinitializer.h"

#include <QDebug>

/**
 * @brief MeshInitializer::MeshInitializer Initializes an empty mesh
 * initializer.
 */
MeshInitializer::MeshInitializer() {}

/**
 * @brief MeshInitializer::constructHalfEdgeMesh Constructs a half-edge mesh
 * from the provided obj file. The half-edge data structure uses smart indexing
 * to do the initialization in a clean manner. This indexing is based on the
 * following paper:
 * https://diglib.eg.org/bitstream/handle/10.1111/cgf14381/v40i8pp057-070.pdf?sequence=1&isAllowed=y
 * @param loadedOBJFile The OBJFile containing all the data of the mesh to
 * construct.
 * @return A half-edge representation of the provided mesh.
 */
Mesh MeshInitializer::constructHalfEdgeMesh(const OBJFile &loadedOBJFile) {
  int numVertices = loadedOBJFile.vertexCoords.size();
  int numFaces = loadedOBJFile.faceValences.size();
  int numHalfEdges = 0;
  for (int f = 0; f < numFaces; f++) {
    numHalfEdges += loadedOBJFile.faceValences[f];
  }

  Mesh mesh;
  mesh.vertices.resize(numVertices);
  mesh.faces.resize(numFaces);
  mesh.halfEdges.resize(numHalfEdges);
  mesh.halfEdges.reserve(2 * numHalfEdges);

  initGeometry(mesh, numVertices, loadedOBJFile.vertexCoords);
  initTopology(mesh, numFaces, loadedOBJFile.faceCoordInd);
  return mesh;
}

/**
 * @brief MeshInitializer::initGeometry Initializes the vertex coordinates and
 * their indices.
 * @param mesh The mesh to initialize.
 * @param numVertices The number of vertices to initialize.
 * @param vertexCoords The vertex coordinates.
 */
void MeshInitializer::initGeometry(Mesh &mesh, int numVertices,
                                   const QVector<QVector3D> &vertexCoords) {
  for (int v = 0; v < numVertices; v++) {
    Vertex *vertex = &mesh.vertices[v];
    vertex->coords = vertexCoords[v];
    vertex->index = v;
  }
}

/**
 * @brief MeshInitializer::initTopology Initializes the half-edges and face
 * data. Makes sure that all the connections are set up correctly.
 * @param mesh The mesh to initialize.
 * @param numFaces The number of faces the mesh will have.
 * @param faceCoordInd A vector containing, for each face, the indices of the
 * vertices.
 */
void MeshInitializer::initTopology(Mesh &mesh, int numFaces,
                                   const QVector<QVector<int>> &faceCoordInd) {
  int h = 0;
  for (int f = 0; f < numFaces; ++f) {
    QVector<int> faceIndices = faceCoordInd[f];
    // Each face ends up with a number of half edges equal to its number of
    // vertices.
    Face *face = &mesh.faces[f];
    face->index = f;
    face->valence = faceIndices.size();
    face->side = &mesh.halfEdges[h];
    for (int i = 0; i < face->valence; ++i) {
      addHalfEdge(mesh, h, face, faceIndices, i);
      // The valence of a vertex is equal to the number of faces it belongs to,
      // so for every face, increment the valence of all its vertices by 1.
      mesh.vertices[faceIndices[i]].valence++;
      h++;
    }
  }
  mesh.edgeCount = edgeList.size();
}

/**
 * @brief MeshInitializer::addHalfEdge Initializes the data of single half-edge
 * in the mesh.
 * @param mesh The mesh to initialize the half-edge in.
 * @param h Index of the half-edge.
 * @param face Face that the half-edge belongs to.
 * @param vertIndices Indices of the vertices that belong to the face this
 * half-edge belongs to.
 * @param i Index within vertIndices vector.
 */
void MeshInitializer::addHalfEdge(Mesh &mesh, int h, Face *face,
                                  const QVector<int> &vertIndices, int i) {
  int faceValence = vertIndices.size();
  int vertIdx = vertIndices[i];
  int nextVertIdx = vertIndices[(i + 1) % faceValence];
  // prev and next
  int prev = h - 1;
  int next = h + 1;
  if (i == 0) {
    // prev = h - 1 + faceValency
    prev += faceValence;
  } else if (i == faceValence - 1) {
    // next = h + 1 - faceValency
    next -= faceValence;
  }
  HalfEdge *halfEdge = &mesh.halfEdges[h];
  halfEdge->index = h;
  halfEdge->origin = &mesh.vertices[vertIdx];
  halfEdge->prev = &mesh.halfEdges[prev];
  halfEdge->next = &mesh.halfEdges[next];
  halfEdge->face = face;
  mesh.vertices[vertIdx].out = halfEdge;

  setTwins(mesh, h, vertIdx, nextVertIdx);
}

/**
 * @brief createUndirectedEdge Creates an undirected edge from two vertex
 * indices.
 * @param v1 First vertex index.
 * @param v2 Second vertex index.
 * @return A pair of v1-v2. Two indices always produce the same pair, regardless
 * of their ordering.
 */
QPair<int, int> createUndirectedEdge(int v1, int v2) {
  // to ensure that edges are consistent, always put the lower index first
  if (v1 > v2) {
    return QPair<int, int>(v2, v1);
  }
  return QPair<int, int>(v1, v2);
}

/**
 * @brief MeshInitializer::setTwins Set the twin properties of the half-edge.
 * Simultaneously updates the edge indices by keeping track of all the edges
 * that have been (partially) covered.
 * @param mesh The mesh the half-edge belongs to.
 * @param h Index of the half-edge.
 * @param vertIdx1 Index of the first vertex of the edge the half-edge belongs
 * to.
 * @param vertIdx2 Index of the second vertex of the edge the half-edge belongs
 * to.
 */
void MeshInitializer::setTwins(Mesh &mesh, int h, int vertIdx1, int vertIdx2) {
  QPair<int, int> currentEdge = createUndirectedEdge(vertIdx1, vertIdx2);

  int edgeIdx = edgeList.indexOf(currentEdge);
  // edge does not exist yet
  if (edgeIdx == -1) {
    // same as doing this after appending and adding -1
    edgeIndices.append(edgeList.size());
    mesh.halfEdges[h].edgeIndex = edgeList.size();
    edgeList.append(currentEdge);
  } else {
    edgeIndices.append(edgeIdx);
    mesh.halfEdges[h].edgeIndex = edgeIdx;
    // edge already existed, meaning there is a twin somewhere earlier in the
    // list of edges
    HalfEdge *twinEdge = &mesh.halfEdges[edgeIndices.indexOf(edgeIdx)];
    mesh.halfEdges[h].twin = twinEdge;
    twinEdge->twin = &mesh.halfEdges[h];
  }
}
