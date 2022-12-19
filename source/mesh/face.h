#ifndef FACE
#define FACE

#include <QVector3D>

// Forward declaration
class HalfEdge;

/**
 * @brief The Face class represent the face data in the half-edge mesh.
 */
class Face {
 public:
  Face();
  Face(HalfEdge* side, int valence, int index);
  void recalculateNormal();
  QVector3D computeNormal() const;
  void debugInfo() const;

  HalfEdge* side;
  int valence;
  int index;
  QVector3D normal;
};

#endif  // FACE
