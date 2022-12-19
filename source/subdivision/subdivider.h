#ifndef SUBDIVIDER_H
#define SUBDIVIDER_H

#include "mesh/mesh.h"

/**
 * @brief The Subdivider class is an abstract class that allows for subdividing
 * meshes.
 */
class Subdivider {
public:
  virtual ~Subdivider();
  virtual Mesh subdivide(Mesh &mesh) const = 0;
};

#endif // SUBDIVIDER_H
