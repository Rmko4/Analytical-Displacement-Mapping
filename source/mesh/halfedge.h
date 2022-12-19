#ifndef HALFEDGE
#define HALFEDGE

// Forward declarations
class Vertex;
class Face;

/**
 * @brief The HalfEdge class represents a directed edge. Each non-boundary edge
 * consists of two half-edges. If the half-edge belongs to a boundary edge, the
 * twin will be a nullpointer.
 */
class HalfEdge {
 public:
  HalfEdge();
  HalfEdge(int index);
  HalfEdge(Vertex* origin, HalfEdge* next, HalfEdge* prev, HalfEdge* twin,
           Face* polygon, int index);

  void debugInfo() const;
  int nextIdx() const;
  int prevIdx() const;
  int twinIdx() const;
  int faceIdx() const;
  int edgeIdx() const;

  bool isBoundaryEdge() const;

  Vertex* origin;
  HalfEdge* next;
  HalfEdge* prev;
  HalfEdge* twin;
  Face* face;
  int index;

  int edgeIndex;
};

#endif  // HALFEDGE
