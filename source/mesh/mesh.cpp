#include "mesh.h"

#include <assert.h>
#include <math.h>

#include <QDebug>

/**
 * @brief Mesh::Mesh Initializes an empty mesh.
 */
Mesh::Mesh() {}

/**
 * @brief Mesh::~Mesh Deconstructor. Clears all the data of the half-edge data.
 */
Mesh::~Mesh() {
  vertices.clear();
  vertices.squeeze();
  halfEdges.clear();
  halfEdges.squeeze();
  faces.clear();
  faces.squeeze();
}

/**
 * @brief Mesh::recalculateNormals Recalculates the face and vertex normals.
 */
void Mesh::recalculateNormals() {
  for (int f = 0; f < numFaces(); f++) {
    faces[f].recalculateNormal();
  }

  vertexNormals.clear();
  vertexNormals.fill({0, 0, 0}, numVerts());

  // normal computation
  for (int h = 0; h < numHalfEdges(); ++h) {
    HalfEdge *edge = &halfEdges[h];
    QVector3D pPrev = edge->prev->origin->coords;
    QVector3D pCur = edge->origin->coords;
    QVector3D pNext = edge->next->origin->coords;

    QVector3D edgeA = (pPrev - pCur);
    QVector3D edgeB = (pNext - pCur);

    float edgeLengths = edgeA.length() * edgeB.length();
    float edgeDot = QVector3D::dotProduct(edgeA, edgeB) / edgeLengths;
    float angle = sqrt(1 - edgeDot * edgeDot);

    vertexNormals[edge->origin->index] +=
        (angle * edge->face->normal) / edgeLengths;
  }

  for (int v = 0; v < numVerts(); ++v) {
    vertexNormals[v] /= vertexNormals[v].length();
  }
}

/**
 * @brief Mesh::computeRegularPatchIndices Computes the indices for regular quad
 * grid patches. The resulting indices are stored in regularPatchIndices.
 */
void Mesh::computeRegularPatchIndices() {
  regularPatchIndices.clear();

  QVector<unsigned int> newRegularPatchIndices;
  newRegularPatchIndices.resize(16);

  // Maps vertex indices to 4x4 row-major ordering.
  QVector<unsigned int> map = {1,  5,  4,  0,  7, 6, 2,  3,
                               14, 10, 11, 15, 8, 9, 13, 12};

  for (int f = 0; f < faces.size(); f++) {
    Face *face = &faces[f];
    if (face->valence != 4) {
      continue;
    }

    bool isRegularPatch = true;
    HalfEdge *currentEdge = face->side;
    // Check the valence of the connected faces
    // and vertices of the central face.
    for (int m = 0; m < face->valence; m++) {
      HalfEdge *twinEdge = currentEdge->twin;
      if (currentEdge->origin->valence != 4 || twinEdge->face->valence != 4 ||
          twinEdge->next->twin->face->valence != 4) {
        isRegularPatch = false;
        break;
      }
      currentEdge = currentEdge->next;
    }

    if (isRegularPatch) {
      HalfEdge *currentInnerEdge = face->side;
      // For rotating around inner quad
      for (int m = 0; m < face->valence; m++) {
        HalfEdge *currentOuterEdge = currentInnerEdge->twin->next->twin;
        // For rotating around outer corner quad
        for (int n = 0; n < face->valence; n++) {
          newRegularPatchIndices[map[m * face->valence + n]] =
              currentOuterEdge->origin->index;
          currentOuterEdge = currentOuterEdge->next;
        }
        currentInnerEdge = currentInnerEdge->next;
      }
      regularPatchIndices.append(newRegularPatchIndices);
    }
  }
}

/**
 * @brief Mesh::extractAttributes Extracts the normals, vertex coordinates and
 * indices into easy-to-access buffers.
 */
void Mesh::extractAttributes() {
  recalculateNormals();

  vertexCoords.clear();
  vertexCoords.reserve(vertices.size());
  for (int v = 0; v < vertices.size(); v++) {
    vertexCoords.append(vertices[v].coords);
  }

  polyIndices.clear();
  polyIndices.reserve(halfEdges.size());
  for (int f = 0; f < faces.size(); f++) {
    HalfEdge *currentEdge = faces[f].side;
    for (int m = 0; m < faces[f].valence; m++) {
      polyIndices.append(currentEdge->origin->index);
      currentEdge = currentEdge->next;
    }
    // append MAX_INT to signify end of face
    polyIndices.append(INT_MAX);
  }
  polyIndices.squeeze();

  quadIndices.clear();
  quadIndices.reserve(halfEdges.size() + faces.size());
  for (int k = 0; k < faces.size(); k++) {
    Face *face = &faces[k];
    HalfEdge *currentEdge = face->side;
    if (face->valence == 4) {
      for (int m = 0; m < face->valence; m++) {
        quadIndices.append(currentEdge->origin->index);
        currentEdge = currentEdge->next;
      }
    }
  }
  quadIndices.squeeze();
}

/**
 * @brief Mesh::numVerts Retrieves the number of vertices.
 * @return The number of vertices.
 */
int Mesh::numVerts() { return vertices.size(); }

/**
 * @brief Mesh::numHalfEdges Retrieves the number of half-edges.
 * @return The number of half-edges.
 */
int Mesh::numHalfEdges() { return halfEdges.size(); }

/**
 * @brief Mesh::numFaces Retrieves the number of faces.
 * @return The number of faces.
 */
int Mesh::numFaces() { return faces.size(); }

/**
 * @brief Mesh::numEdges Retrieves the number of edges.
 * @return The number of edges.
 */
int Mesh::numEdges() { return edgeCount; }
