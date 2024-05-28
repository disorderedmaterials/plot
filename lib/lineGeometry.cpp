#include "lineGeometry.h"

LineGeometry::LineGeometry() { updateData(); }

void LineGeometry::updateData()
{
    clear();

    int stride = 3 * sizeof(float);

    QByteArray vertexData(3 * stride, Qt::Initialization::Uninitialized);
    float *p = reinterpret_cast<float *>(vertexData.data());

    // a triangle, front face = counter-clockwise
    *p++ = -1.0f; *p++ = -1.0f; *p++ = 0.0f;
    *p++ = 1.0f; *p++ = -1.0f; *p++ = 0.0f;
    *p++ = 0.0f; *p++ = 1.0f; *p++ = 0.0f;

    setVertexData(vertexData);
    setStride(stride);
    setBounds(QVector3D(-1.0f, -1.0f, 0.0f), QVector3D(+1.0f, +1.0f, 0.0f));

    setPrimitiveType(QQuick3DGeometry::PrimitiveType::Triangles);

    addAttribute(QQuick3DGeometry::Attribute::PositionSemantic,
                 0,
                 QQuick3DGeometry::Attribute::F32Type);
}
