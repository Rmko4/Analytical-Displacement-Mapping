#include "util.h"

#include <QDebug>

/**
 * @brief calcBoundingBoxScale Calculates the scale with which to scale the
 * provided coordinates for all of them to fit inside a bounding box(cube) of
 * the desired scale.
 * @param coords The coordinates to fit in the bounding box
 * @param desiredScale The scale of the bounding box. Scale=1 will result in a
 * unit bounding box.
 * @return The scale with which to transform the coordinates to fit in the
 * bounding box.
 */
float calcBoundingBoxScale(const QVector<QVector3D> coords,
                           const float desiredScale) {
  QVector3D minCoord = coords[0];
  QVector3D maxCoord = coords[0];
  for (int i = 0; i < coords.size(); ++i) {
    QVector3D coord = coords[i];
    minCoord.setX(std::min(coord.x(), minCoord.x()));
    minCoord.setY(std::min(coord.y(), minCoord.y()));
    minCoord.setZ(std::min(coord.z(), minCoord.z()));

    maxCoord.setX(std::max(coord.x(), maxCoord.x()));
    maxCoord.setY(std::max(coord.y(), maxCoord.y()));
    maxCoord.setZ(std::max(coord.z(), maxCoord.z()));
  }
  QVector3D dims = maxCoord - minCoord;
  return desiredScale / std::min(dims.x(), dims.y());
}
