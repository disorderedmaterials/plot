#include "lineentity.h"

#include <Qt3DExtras/QPhongMaterial>

LineGeometry::LineGeometry(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent), geometry_(this), geometryRenderer_(this)
{
    // TEST
    QVector3D start(0,0,0), end(10,10,10);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();

    auto *buf = new Qt3DCore::QBuffer(&geometry_);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DCore::QAttribute(&geometry_);
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry_.addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DCore::QBuffer(&geometry_);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DCore::QAttribute(&geometry_);
    indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry_.addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    geometryRenderer_.setGeometry(&geometry_);
    geometryRenderer_.setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(parent);
    material->setAmbient(Qt::blue);

    // entity
    addComponent(&geometryRenderer_);
    addComponent(material);
}
