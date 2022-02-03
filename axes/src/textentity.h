#pragma once

#include "cuboid.h"
#include "metrics.h"
#include <QFontMetrics>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QExtrudedTextMesh>

class TextEntity : public Qt3DCore::QEntity
{
    public:
    TextEntity(Qt3DCore::QNode *parent = nullptr);
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
    // Calculate anchor point coordinate
    QVector3D anchorPointCoordinate() const;
    // Update translation
    void updateTranslation();

    public:
    // Set text
    void setText(const QString &text);
    // Set font
    void setFont(const QFont &font);
    // Set anchor point
    void setAnchorPoint(MildredMetrics::AnchorPoint anchor);
    // Set anchor position
    void setAnchorPosition(QVector3D p);
    // Return local bounding cuboid taking anchor point into account
    static Cuboid boundingCuboid(const QFont &font, const QString &text, QVector3D anchorPosition,
                                 MildredMetrics::AnchorPoint anchorPoint, float depth = 0.1f);
};
