#pragma once

#include "classes/cuboid.h"
#include "classes/metrics.h"
#include "material.h"
#include <QFontMetrics>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QExtrudedTextMesh>

namespace Mildred
{
//! TextEntity represents a 3D text entity
/*!
 * TextEntity represents a renderable entity displaying a specified string with a given transform.
 */
class TextEntity : public Qt3DCore::QEntity
{
    public:
    TextEntity(Qt3DCore::QNode *parent = nullptr, QString text = "");
    ~TextEntity() = default;

    private:
    // Collective positional transform
    Qt3DCore::QTransform *positionalTransform_{nullptr};
    // Material
    RenderableMaterial *material_{nullptr};
    // Main entity
    Qt3DCore::QEntity *textEntity_{nullptr};
    // Text mesh
    Qt3DExtras::QExtrudedTextMesh *textMesh_{nullptr};
    // Transform
    Qt3DCore::QTransform *textTransform_{nullptr};
    // Bounding box and anchor point entities
    LineEntity *boundingBoxEntity_{nullptr}, *anchorPointEntity_{nullptr};
    // Transforms for bounding box and anchor point entities
    Qt3DCore::QTransform *boundingBoxTransform_{nullptr};
    double widthExtent_{0};

    public:
    // Set text material
    void setMaterial(RenderableMaterial *material);

    /*
     * Definition
     */
    private:
    // Anchor point for entity (= 0,0,0)
    MildredMetrics::AnchorPoint anchorPoint_{MildredMetrics::AnchorPoint::TopMiddle};
    // Requested anchor position
    QVector3D anchorPosition_{0.0, 0.0, 0.0};

    private:
    // Update translation
    void updateTranslation();

    public:
    // Set text
    void setText(const QString &text);
    // Return current text
    QString text() const;
    // Set font
    void setFont(const QFont &font);
    // Set anchor point
    void setAnchorPoint(MildredMetrics::AnchorPoint anchor);
    // Set anchor position
    void setAnchorPosition(QVector3D p);
    // Get anchor position
    QVector3D anchorPosition() const;
    // Get width extent
    float getWidthExtent();
    // Return simple bounding cuboid for text, along with baseline descent from font metrics
    static std::pair<Cuboid, int> boundingCuboid(const QFont &font, const QString &text, float depth = 0.1f);
    // Return bounding cuboid with translation and anchor point applied, and required translation vector for text mesh
    static std::pair<Cuboid, QVector3D> boundingCuboid(const QFont &font, const QString &text, QVector3D anchorPosition,
                                                       MildredMetrics::AnchorPoint anchorPoint, float depth = 0.1f);
};
} // namespace Mildred