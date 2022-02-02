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
     *
     */
    QFontMetrics fontMetrics(mesh_->font());
    auto boundingRect = fontMetrics.boundingRect(mesh_->text());

    switch (anchorPoint_)
    {
        case (MildredMetrics::AnchorPoint::TopLeft):
            return {0.0, float(mesh_->font().pointSizeF()), 0.0f};
        case (MildredMetrics::AnchorPoint::TopMiddle):
            return {float(boundingRect.width() * 0.5), float(mesh_->font().pointSizeF()), 0.0f};
        case (MildredMetrics::AnchorPoint::TopRight):
            return {float(boundingRect.width()), float(mesh_->font().pointSizeF()), 0.0f};
        case (MildredMetrics::AnchorPoint::MiddleLeft):
            return {0.0, float(mesh_->font().pointSizeF() * 0.5), 0.0f};
        case (MildredMetrics::AnchorPoint::Middle):
            return {float(boundingRect.width() * 0.5), float(mesh_->font().pointSizeF() * 0.5), 0.0f};
        case (MildredMetrics::AnchorPoint::MiddleRight):
            return {float(boundingRect.width()), float(mesh_->font().pointSizeF() * 0.5), 0.0f};
        case (MildredMetrics::AnchorPoint::BottomLeft):
            return {0.0, 0.0, 0.0f};
        case (MildredMetrics::AnchorPoint::BottomMiddle):
            return {float(boundingRect.width() * 0.5), 0.0, 0.0f};
        case (MildredMetrics::AnchorPoint::BottomRight):
            return {float(boundingRect.width()), 0.0, 0.0f};
        default:
            throw(std::runtime_error("Unhandled anchor point."));
    }
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
