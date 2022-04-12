#include "entities/line.h"

using namespace Mildred;

//! Construct a new LineEntity
/*!
 * Creates an empty LineEntity with the specified drawing @param primitiveType. This function creates the necessary buffers and
 * attributes in order to be able to generate a line primitive by providing the necessary vertices and indices piecewise.
 */
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

//! Append vertex to cached data
/*!
 * Append the vertex @param v to the cached vertices. The buffer objects (and hence the display primitive) are not regenerated
 * until a call to the finalise() method is made.
 */
void LineEntity::addVertex(QVector3D v) { cachedVertices_.emplace_back(v); }

//! Append verteices to cached data
/*!
 * Append the vector of @param vertices to the cached vertices. The buffer objects (and hence the display primitive) are not
 * regenerated until a call to the finalise() method is made.
 */
void LineEntity::addVertices(const std::vector<QVector3D> &vertices)
{
    std::copy(vertices.begin(), vertices.end(), std::back_inserter(cachedVertices_));
}

//! Append index to cached data
/*!
 * Append the index @param i to the list of cached indices. The buffer objects (and hence the display primitive) are not
 * regenerated until a call to the finalise() method is made.
 */
void LineEntity::addIndex(unsigned int i) { cachedIndices_.push_back(i); }

//! Append indices to cached data
/*!
 * Append the vector of @param indices to the list of cached indices. The buffer objects (and hence the display primitive) are
 * not regenerated until a call to the finalise() method is made.
 */
void LineEntity::addIndices(const std::vector<unsigned int> &indices)
{
    std::copy(indices.begin(), indices.end(), std::back_inserter(cachedIndices_));
}

//! Set basic (sequential) indices
void LineEntity::setBasicIndices()
{
    cachedIndices_.resize(cachedVertices_.size());
    std::iota(cachedIndices_.begin(), cachedIndices_.end(), 0);
}

//! Finalise geometry from cached data
/*!
 * Finalise the entity making it ready for rendering, creating the necessary buffers from the cached vertices and indices.
 *
 * Once the geometry is constructed the cached vertex and index data is cleared, permitting new data to be added and the entity
 * to be recreated again at a later date.
 */
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

//! Clear geometry
/*!
 * Remove all cached vertex and index data and set the expected size of the geometry's attributed to zero, effectively making
 * the entity render nothing to screen.
 */
void LineEntity::clear()
{
    vertexAttribute_.setCount(0);
    indexAttribute_.setCount(0);
    cachedVertices_.clear();
    cachedIndices_.clear();
}
