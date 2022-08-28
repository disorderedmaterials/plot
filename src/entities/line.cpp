#include "entities/line.h"

using namespace Mildred;

//! Construct a new LineEntity
/*!
 * Creates an empty LineEntity with the specified drawing @param primitiveType. This function creates the necessary buffers and
 * attributes in order to be able to generate a line primitive by providing the necessary vertices and indices piecewise.
 */
LineEntity::LineEntity(Qt3DCore::QNode *parent, Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType)
    : Qt3DCore::QEntity(parent), geometry_(this), geometryRenderer_(this), vertexBuffer_(&geometry_),
      vertexAttribute_(&geometry_), indexBuffer_(&geometry_), indexAttribute_(&geometry_), colourBuffer_(&geometry_),
      colourAttribute_(&geometry_)
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

    // Set up the index attribute
    colourAttribute_.setName(Qt3DCore::QAttribute::defaultColorAttributeName());
    colourAttribute_.setVertexBaseType(Qt3DCore::QAttribute::Float);
    colourAttribute_.setVertexSize(4);
    colourAttribute_.setBuffer(&colourBuffer_);
    colourAttribute_.setByteStride(4 * sizeof(float));
    colourAttribute_.setCount(0);

    // Set up geometry and renderer
    geometry_.addAttribute(&vertexAttribute_);
    geometry_.addAttribute(&indexAttribute_);
    geometry_.addAttribute(&colourAttribute_);

    geometryRenderer_.setGeometry(&geometry_);
    geometryRenderer_.setPrimitiveType(primitiveType);

    // Enable primitive restarting and define the restart index value.
    // This allows us to enable line breaks, by adding indices with the value
    // of the restart value.
    geometryRenderer_.setPrimitiveRestartEnabled(true);
    geometryRenderer_.setRestartIndexValue(-1);

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

//! Append vertex to cached data
/*!
 * Append the vertex defined by the coordinates @param x, @param y, and @param z to the cached vertices. The buffer objects (and hence the display primitive) are not regenerated
 * until a call to the finalise() method is made.
 */
void LineEntity::addVertex(double x, double y, double z) { cachedVertices_.emplace_back(QVector3D{float(x), float(y), float(z)});}

//! Append vertex and colour to cached data
/*!
 * Append the vertex @param v to the cached vertices, with the specific @param colour. The buffer objects (and hence the display
 * primitive) are not regenerated until a call to the finalise() method is made.
 */
void LineEntity::addVertex(QVector3D v, QColor colour)
{
    cachedVertices_.emplace_back(v);
    cachedVertexColours_.emplace_back(colour);
}

//! Append vertices to cached data
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

    // Convert index data into a QByteArray
    QByteArray indexBytes;
    indexBytes.resize(cachedIndices_.size() * sizeof(unsigned int));
    auto *indices = reinterpret_cast<unsigned int *>(indexBytes.data());
    for (const auto i : cachedIndices_)
        *indices++ = i;
    indexBuffer_.setData(indexBytes);
    indexAttribute_.setCount(cachedIndices_.size());

    // Convert colour cache into a QByteArray
    if (cachedVertices_.size() == cachedVertexColours_.size())
    {
        QByteArray colourBytes;
        colourBytes.resize(cachedVertexColours_.size() * 4 * sizeof(float));
        auto *colours = reinterpret_cast<float *>(colourBytes.data());
        for (const auto &c : cachedVertexColours_)
        {
            *colours++ = c.redF();
            *colours++ = c.greenF();
            *colours++ = c.blueF();
            *colours++ = c.alphaF();
        }
        colourBuffer_.setData(colourBytes);
        colourAttribute_.setCount(cachedVertexColours_.size());
    }
    else
        colourAttribute_.setCount(0);

    cachedVertices_.clear();
    cachedIndices_.clear();
    cachedVertexColours_.clear();
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
    colourAttribute_.setCount(0);
    cachedVertices_.clear();
    cachedIndices_.clear();
    cachedVertexColours_.clear();
}
