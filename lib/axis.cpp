#include "axis.h"
#include <algorithm>

#include <iostream>

Axis::Axis() : minimum_(-1), maximum_(1), thickness_(0.001), direction_(false) {
  updateData();
  connect(this, &Axis::dataChanged, this, &Axis::updateData);
}

void Axis::updateData()
{
  clear();

  int stride = 3 * sizeof(float);

  QByteArray vertexData(6 * stride, Qt::Initialization::Uninitialized);
  float *p = reinterpret_cast<float *>(vertexData.data());

  *p++ = -1.0;
  *p++ = -1.0;
  *p++ = 0;
  *p++ = 1.0;
  *p++ = -1.0;
  *p++ = 0;
  *p++ = 1.0;
  *p++ = -1.0 + thickness_;
  *p++ = 0;

  *p++ = 1.0;
  *p++ = -1.0 + thickness_;
  *p++ = 0;
  *p++ = -1.0;
  *p++ = -1.0 + thickness_;
  *p++ = 0;
  *p++ = -1.0;
  *p++ = -1.0;
  *p++ = 0;

  setVertexData(vertexData);
  setStride(stride);
  setBounds(QVector3D(-1.0f, -1.0f, 0.0f), QVector3D(+1.0f, -1.0f + thickness_, 0.0f));

  addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
               0,
               QQuick3DGeometry::Attribute::F32Type);

  update();
  std::cout << "Start Data" << std::endl;
}
