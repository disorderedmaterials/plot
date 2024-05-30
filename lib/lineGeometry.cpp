#include "lineGeometry.h"
#include <algorithm>

#include <iostream>

LineGeometry::LineGeometry() {
  generateSines();
  updateData();
  connect(this, &LineGeometry::dataChanged, this, &LineGeometry::updateData);
}

void LineGeometry::generateSines() {
    const int N = 200;

    xs_.resize(N);
    ys_.resize(N);

    std::iota(xs_.begin(), xs_.end(), 0);
    std::transform(xs_.begin(), xs_.end(), xs_.begin(), [](auto x) {return -1.0 + 2 * x / N;});
    std::transform(xs_.begin(), xs_.end(), ys_.begin(), [](auto x) {return sin(3 * M_PI * x);});
}

void LineGeometry::updateData()
{
    clear();


    const int N = xs_.length();
    if (N < 2 || ys_.length() < 2) return;
    int stride = 3 * sizeof(float);

    QByteArray vertexData(6 * N * stride, Qt::Initialization::Uninitialized);
    float *p = reinterpret_cast<float *>(vertexData.data());

    std::vector<float> angles(xs_.length());
    // Calculate derivatives
    for (int i=1; i<N-1; ++i) {
      auto angle = atan2(ys_[i+1] - ys_[i-1], xs_[i+1] - xs_[i-1]);
      angles[i] = angle + M_PI/2;
    }
    angles[0] = M_PI/2;
    angles[N-1] = M_PI/2;

    for (int i=0; i<N - 1; i++) {
      *p++ = xs_[i] + cos(angles[i]) * thickness_;
      *p++ = ys_[i] + sin(angles[i]) * thickness_;
      *p++ = 0.0f;
      *p++ = xs_[i] - cos(angles[i]) * thickness_;
      *p++ = ys_[i] - sin(angles[i]) * thickness_;
      *p++ = 0.0f;
      *p++ = xs_[i+1] - cos(angles[i+1]) * thickness_;
      *p++ = ys_[i+1] - sin(angles[i+1]) * thickness_;
      *p++ = 0.0f;

      *p++ = xs_[i+1] - cos(angles[i+1]) * thickness_;
      *p++ = ys_[i+1] - sin(angles[i+1]) * thickness_;
      *p++ = 0.0f;
      *p++ = xs_[i+1] + cos(angles[i+1]) * thickness_;
      *p++ = ys_[i+1] + sin(angles[i+1]) * thickness_;
      *p++ = 0.0f;
      *p++ = xs_[i] + cos(angles[i]) * thickness_;
      *p++ = ys_[i] + sin(angles[i]) * thickness_;
      *p++ = 0.0f;
    }

    setVertexData(vertexData);
    setStride(stride);
    setBounds(QVector3D(-1.0f, -1.0f, 0.0f), QVector3D(+1.0f, +1.0f, 0.0f));

    // setPrimitiveType(QQuick3DGeometry::PrimitiveType::TriangleStrip);

    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                 0,
                 QQuick3DGeometry::Attribute::F32Type);

    update();
}
