#include "lineentity.h"

LineEntity::LineEntity(Qt3DCore::QNode *parent, Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType)
    : Qt3DCore::QEntity(parent), geometry_(this), geometryRenderer_(this), vertexBuffer_(&geometry_),
      vertexAttribute_(&geometry_), indexBuffer_(&geometry_), indexAttribute_(&geometry_)
{
    // Set up the vertex attribute
    vertexAttribute_.setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    vertexAttribute_.setVertexBaseType(Qt3DCore::QAttribute::Float);
    vertexAttribute_.setVertexSize(3);
    vertexAttribute_.setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    vertexAttribute_.setBuffer(&vertexBuffer_);
    vertexAttribute_.setByteStride(3 * sizeof(float));
    vertexAttribute_.setCount(0);

    // Set up the index attribute
    indexAttribute_.setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);
    indexAttribute_.setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    indexAttribute_.setBuffer(&indexBuffer_);
    indexAttribute_.setCount(0);

    // Set up geometry and renderer
    geometry_.addAttribute(&vertexAttribute_);
    geometry_.addAttribute(&indexAttribute_);

    geometryRenderer_.setGeometry(&geometry_);
    geometryRenderer_.setPrimitiveType(primitiveType);

    // Set up entity
    addComponent(&geometryRenderer_);
}

/*
 * Convenience Functions
 */

// Append vertices to cached data
void LineEntity::addVertex(QVector3D v) { cachedVertices_.emplace_back(v); }
void LineEntity::addVertices(const std::vector<QVector3D> &vertices)
{
    std::copy(vertices.begin(), vertices.end(), std::back_inserter(cachedVertices_));
}

// Append indices to cached data
void LineEntity::setBasicIndices()
{
    cachedIndices_.resize(cachedVertices_.size());
    std::iota(cachedIndices_.begin(), cachedIndices_.end(), 0);
}
void LineEntity::addIndex(unsigned int i) { cachedIndices_.push_back(i); }
void LineEntity::addIndices(const std::vector<unsigned int> &indices)
{
    std::copy(indices.begin(), indices.end(), std::back_inserter(cachedIndices_));
}

// Finalise geometry from cached data
void LineEntity::finalise()
{
    // Convert vertex cache into a QByteArray
    QByteArray vertexBytes;
    vertexBytes.resize(cachedVertices_.size() * 3 * sizeof(float));
    auto *vertices = reinterpret_cast<float *>(vertexBytes.data());
    for (const auto &v : cachedVertices_)
    {
        *vertices++ = v.x();
        *vertices++ = v.y();
        *vertices++ = v.z();
    }
    vertexBuffer_.setData(vertexBytes);
    vertexAttribute_.setCount(cachedVertices_.size());
    cachedVertices_.clear();

    // Convert index data into a QByteArray
    QByteArray indexBytes;
    indexBytes.resize(cachedIndices_.size() * sizeof(unsigned int));
    auto *indices = reinterpret_cast<unsigned int *>(indexBytes.data());
    for (const auto i : cachedIndices_)
        *indices++ = i;
    indexBuffer_.setData(indexBytes);
    indexAttribute_.setCount(cachedIndices_.size());
    cachedIndices_.clear();
}

// Clear geometry
void LineEntity::clear()
{
    vertexAttribute_.setCount(0);
    indexAttribute_.setCount(0);
    cachedVertices_.clear();
    cachedIndices_.clear();
}
