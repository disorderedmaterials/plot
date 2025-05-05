#pragma once

#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

namespace Mildred
{
//! LineEntity represents a renderable lines / wireframe object
/*!
 * LineEntity encapsulates the buffers and attributes necessary to provide a line-drawn QEntity for use in a Qt3D scenegraph.
 */
class LineEntity : public Qt3DCore::QEntity
{
    public:
    LineEntity(Qt3DCore::QNode *parent = nullptr,
               Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType = Qt3DRender::QGeometryRenderer::LineStrip);
    ~LineEntity() = default;

    private:
    // Primitive geometry
    Qt3DCore::QGeometry geometry_;
    // Renderer for primitive geometry
    Qt3DRender::QGeometryRenderer geometryRenderer_;
    // Buffers and attributes
    Qt3DCore::QBuffer vertexBuffer_;
    Qt3DCore::QAttribute vertexAttribute_;
    Qt3DCore::QBuffer indexBuffer_;
    Qt3DCore::QAttribute indexAttribute_;
    Qt3DCore::QBuffer colourBuffer_;
    Qt3DCore::QAttribute colourAttribute_;

    /*
     * Convenience Functions
     */
    private:
    // Cached vertices
    QVector<QVector3D> cachedVertices_;
    // Cached indices
    QVector<unsigned int> cachedIndices_;
    // Cached colours
    QVector<QColor> cachedVertexColours_;

    public:
    // Append vertices to cached data
    void addVertex(QVector3D v);
    void addVertex(double x, double y, double z);
    void addVertex(QVector3D v, QColor colour);
    void addVertices(const std::vector<QVector3D> &vertices);
    // Append indices to cached data
    void addIndex(unsigned int i);
    void addIndices(const std::vector<unsigned int> &indices);
    // Set basic (sequential) indices
    void setBasicIndices();
    // Finalise geometry from cached data
    void finalise();
    // Clear geometry
    void clear();
};
} // namespace Mildred