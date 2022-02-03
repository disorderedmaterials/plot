#include "textentity.h"

TextEntity::TextEntity(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent)
{
    // Create an extruded text mesh
    mesh_ = new Qt3DExtras::QExtrudedTextMesh(this);
    mesh_->setFont(QFont("monospace", 10.0));
    mesh_->setDepth(0.1);
    addComponent(mesh_);

    // Create a local transform and scale the text from unit height to the point size
    transform_ = new Qt3DCore::QTransform(this);
    transform_->setScale3D({10.0f, 10.0f, 1.0f});
    addComponent(transform_);
}

/*
 * Definition
 */

// Calculate anchor point coordinate
QVector3D TextEntity::anchorPointCoordinate() const
{
    /*
     * Remember that:
     *   - The QExtrudedTextMesh has origin (0,0,0) at the font baseline at the leftmost point.
     *   - The boundingRect returned by QFontMetrics is in screen coordinates so has -ve top
     */
    QFontMetrics fontMetrics(mesh_->font());
    auto boundingRect = fontMetrics.boundingRect(mesh_->text());
    auto anchorFrac = MildredMetrics::anchorLocation(anchorPoint_);
    return {float(boundingRect.width() * anchorFrac.x()), float(mesh_->font().pointSizeF() * anchorFrac.y()), 0.0};
}

// Update translation
void TextEntity::updateTranslation()
{
    // Set the translation vector so that the defined anchor point is located at the desired position
    transform_->setTranslation(anchorPosition_ - anchorPointCoordinate());
}

// Set text
void TextEntity::setText(const QString &text)
{
    mesh_->setText(text);

    updateTranslation();
}

// Set font
void TextEntity::setFont(const QFont &font)
{
    mesh_->setFont(font);

    // Set the scale factor to be consistent with the font point size
    transform_->setScale3D({float(font.pointSizeF()), float(font.pointSizeF()), 1.0});

    updateTranslation();
}

// Set anchor point
void TextEntity::setAnchorPoint(MildredMetrics::AnchorPoint anchor)
{
    anchorPoint_ = anchor;

    updateTranslation();
}

// Set anchor position
void TextEntity::setAnchorPosition(QVector3D p)
{
    anchorPosition_ = p;

    updateTranslation();
}

// Return bounding cuboid
Cuboid TextEntity::boundingCuboid(const QFont &font, const QString &text, QVector3D anchorPosition,
                                  MildredMetrics::AnchorPoint anchorPoint, float depth)
{
    QFontMetrics fontMetrics(font);
    auto boundingRect = fontMetrics.boundingRect(text);
    auto anchorFrac = MildredMetrics::anchorLocation(anchorPoint);

    QRectF boundingRectF(boundingRect);
    boundingRectF.translate(boundingRect.width() * anchorFrac.x(), font.pointSizeF() * anchorFrac.y());

    // Translate the bounding rect according to the anchor and position
    QVector3D v1(anchorPosition);
    v1 += {float(boundingRectF.left()), float(boundingRectF.bottom()), 0.0};
    return {v1, v1 + QVector3D(float(boundingRectF.width()), float(boundingRectF.height()), depth)};
}
