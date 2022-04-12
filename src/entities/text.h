#pragma once

#include "classes/cuboid.h"
#include "classes/metrics.h"
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
    // Text mesh
    Qt3DExtras::QExtrudedTextMesh *mesh_;
    // Transform
    Qt3DCore::QTransform *transform_;

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
    // Set axisTickLabelFont
    void setFont(const QFont &font);
    // Set anchor point
    void setAnchorPoint(MildredMetrics::AnchorPoint anchor);
    // Set anchor position
    void setAnchorPosition(QVector3D p);
    // Return simple bounding cuboid for text
    static Cuboid boundingCuboid(const QFont &font, const QString &text, float depth = 0.1f);
    // Return bounding cuboid with translation and anchor point applied
    static Cuboid boundingCuboid(const QFont &font, const QString &text, QVector3D anchorPosition,
                                 MildredMetrics::AnchorPoint anchorPoint, float depth = 0.1f);
};
} // namespace Mildred