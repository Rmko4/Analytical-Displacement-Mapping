#include "vertex.h"

#include <math.h>

#include <QDebug>

#include "face.h"
#include "halfedge.h"

/**
 * @brief Vertex::Vertex Initializes an empty vertex.
 */
Vertex::Vertex() {
  coords = QVector3D();
  out = nullptr;
  valence = 0;
  index = 0;
}

/**
 * @brief Vertex::Vertex Initializes a vertex with its data.
 * @param coords The coordinates of this vertex.
 * @param out One of the half-edges that has this vertex as its origin.
 * @param valence The number of outgoing edges from this vertex.
 * @param index The index of this vertex in the vector of vertices within
 * the mesh.
 */
Vertex::Vertex(QVector3D coords, HalfEdge *out, int valence, int index) {
  this->coords = coords;
  this->out = out;
  this->valence = valence;
  this->index = index;
}

/**
 * @brief Vertex::nextBoundaryHalfEdge Every boundary vertex should have two
 * connecting boundary half-edges (provided the mesh is manifold). One of those
 * originates from this vertex, the other one points to this vertex. This
 * function retrieves the one that originates from this vertex. It does so by
 * following the twin->next loop. Only works if this vertex is a boundary
 * vertex.
 * @return A boundary half-edge that originates from this vertex.
 */
HalfEdge *Vertex::nextBoundaryHalfEdge() const {
  HalfEdge *h = out;
  while (!h->isBoundaryEdge()) {
    h = h->twin->next;
  }
  return h;
}

/**
 * @brief Vertex::prevBoundaryHalfEdge Every boundary vertex should have two
 * connecting boundary half-edges (provided the mesh is manifold). One of those
 * originates from this vertex, the other one points to this vertex. This
 * function retrieves the boundary half-edge that points to this vertex. It does
 * so by following the prev->twin loop. Only works if this vertex is a boundary
 * vertex.
 * @return A boundary half-edge that points to this vertex.
 */
HalfEdge *Vertex::prevBoundaryHalfEdge() const {
  HalfEdge *h = out->prev;
  while (!h->isBoundaryEdge()) {
    h = h->twin->prev;
  }
  return h;
}

/**
 * @brief Vertex::isBoundaryVertex Determines whether this vertex lies on a
 * boundary or not.
 * @return True if the vertex lies on a boundary; false otherwise.
 */
bool Vertex::isBoundaryVertex() const {
  HalfEdge *h = out;
  if (h->isBoundaryEdge()) {
    return true;
  }
  HalfEdge *hNext = h->twin->next;
  while (hNext != h) {
    if (hNext->isBoundaryEdge()) {
      return true;
    }
    hNext = hNext->twin->next;
  }
  return false;
}

/**
 * @brief Vertex::recalculateValence Recalculates the valence of this vertex.
 */
void Vertex::recalculateValence() {
  HalfEdge *currentEdge = out->prev->twin;
  int n = 1;
  while (currentEdge != nullptr && currentEdge != out) {
    currentEdge = currentEdge->prev->twin;
    n++;
  }
  currentEdge = out->twin;
  while (currentEdge != nullptr && currentEdge->next != out) {
    currentEdge = currentEdge->next->twin;
    n++;
  }
  valence = n;
}

/**
 * @brief Vertex::debugInfo Prints some debug info of this vertex.
 */
void Vertex::debugInfo() const {
  qDebug() << "Vertex at Index =" << index << "Coords =" << coords
           << "Out =" << out << "Valence =" << valence;
}
