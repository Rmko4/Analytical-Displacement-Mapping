#include "halfedge.h"

#include <QDebug>

#include "face.h"

/**
 * @brief HalfEdge::HalfEdge Initializes an empty half-edge.
 */
HalfEdge::HalfEdge() {
  origin = nullptr;
  next = nullptr;
  prev = nullptr;
  twin = nullptr;
  face = nullptr;
  index = 0;
}

/**
 * @brief HalfEdge::HalfEdge Initializes a half-edge with just an index.
 * @param index The index of this half-edge in the vector of half-edges within
 * the mesh.
 */
HalfEdge::HalfEdge(int index) {
  origin = nullptr;
  next = nullptr;
  prev = nullptr;
  twin = nullptr;
  face = nullptr;
  this->index = index;
}

/**
 * @brief HalfEdge::HalfEdge Initializes a half-edge with its properties.
 * @param origin The vertex this half-edge originates from.
 * @param next The next half-edge in the cycle.
 * @param prev The previous half-edge in the cycle.
 * @param twin The twin of this half-edge. Null pointer if the edge is a
 * boundary edge.
 * @param face The face that this half-edge belongs to.
 * @param index The index of this half-edge in the vector of half-edges within
 * the mesh.
 */
HalfEdge::HalfEdge(Vertex *origin, HalfEdge *next, HalfEdge *prev,
                   HalfEdge *twin, Face *face, int index) {
  this->origin = origin;
  this->next = next;
  this->prev = prev;
  this->twin = twin;
  this->face = face;
  this->index = index;
}

/**
 * @brief HalfEdge::debugInfo Prints some debug info of this half-edge.
 */
void HalfEdge::debugInfo() const {
  qDebug() << "HalfEdge at Index =" << index << "Target =" << origin
           << "Next =" << next << "Prev =" << prev << "Twin =" << twin
           << "Poly =" << face;
}

/**
 * @brief HalfEdge::nextIdx Calculates the index of the next half-edge. Can be
 * invoked regardless of whether the next is set or not, due to the rules the
 * indexing follows.
 * @return The index of the next half-edge.
 */
int HalfEdge::nextIdx() const {
  if (next != nullptr) {
    return next->index;
  }
  if (index < 0)
    return -1;
  return index % 4 == 3 ? index - 3 : index + 1;
}

/**
 * @brief HalfEdge::prevIdx Calculates the index of the previous half-edge. Can
 * be invoked regardless of whether the prev is set or not, due to the rules the
 * indexing follows.
 * @return The index of the previous half-edge.
 */
int HalfEdge::prevIdx() const {
  if (prev != nullptr) {
    return prev->index;
  }
  if (index < 0)
    return -1;
  return index % 4 == 0 ? index + 3 : index - 1;
}

/**
 * @brief HalfEdge::twinIdx Calculates the index of the twin half-edge. If this
 * edge lives on a boundary edge, it will return -1.
 * @return The index of the twin half-edge. -1 if there is no twin.
 */
int HalfEdge::twinIdx() const {
  if (twin == nullptr) {
    return -1;
  }
  return twin->index;
}

/**
 * @brief HalfEdge::faceIdx Calculates the index of face this half-edge belongs
 * to. Can be invoked regardless of whether the face is set or not, due to the
 * rules the indexing follows.
 * @return The index of the face this half-edge belongs to. Returns -1 if the
 * index is negative.
 */
int HalfEdge::faceIdx() const {
  if (face != nullptr) {
    return face->index;
  }
  if (index < 0)
    return -1;
  return index / 4;
}

/**
 * @brief HalfEdge::edgeIdx Retrieves the index of the (undirected) edge that
 * this half-edge belongs to.
 * @return The index of the (undirected) edge this half-edge belongs to.
 */
int HalfEdge::edgeIdx() const { return edgeIndex; }

/**
 * @brief HalfEdge::isBoundaryEdge Determines whether this edge is a boundary
 * edge or not.
 * @return True if this half-edge lives on a boundary edge; false otherwise.
 */
bool HalfEdge::isBoundaryEdge() const { return twin == nullptr; }
