#ifndef UTIL_H
#define UTIL_H

#include <QVector3D>
#include <QVector>

float calcBoundingBoxScale(const QVector<QVector3D> coords,
                           const float desiredScale = 1.0f);

#endif  // UTIL_H
