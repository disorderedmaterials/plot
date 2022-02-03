#include "textentity.h"

TextEntity::TextEntity(Qt3DCore::QNode *parent, QString text) : Qt3DCore::QEntity(parent)
{
    // Create an extruded text mesh
    mesh_ = new Qt3DExtras::QExtrudedTextMesh(this);
    mesh_->setFont(QFont("monospace", 10.0));
    mesh_->setText(text);
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

// Update translation
void TextEntity::updateTranslation()
{
    // Set the translation vector so that the defined anchor point is located at the desired position
    auto anchorFrac = MildredMetrics::anchorLocation(anchorPoint_);
    auto textCuboid = boundingCuboid(mesh_->font(), mesh_->text(), mesh_->depth());
    transform_->setTranslation(anchorPosition_ -
                               QVector3D(textCuboid.xExtent() * anchorFrac.x(), textCuboid.yExtent() * anchorFrac.y(), 0.0));
}

// Set text
void TextEntity::setText(const QString &text)
{
    mesh_->setText(text);

    updateTranslation();
}

// Return current text
QString TextEntity::text() const { return mesh_->text(); }

// Set axisTickLabelFont
void TextEntity::setFont(const QFont &font)
{
    mesh_->setFont(font);

    // Set the scale factor to be consistent with the axisTickLabelFont point size
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

// Return simple bounding cuboid for text
Cuboid TextEntity::boundingCuboid(const QFont &font, const QString &text, float depth)
{
    QFontMetrics fontMetrics(font);
    const auto boundingRect = fontMetrics.boundingRect(text);
    /*
     * Take the values out of boundingRect (which refers to Qt's coordinate system where 0,0 is top left) so we understand what
     * we're doing. We need to negate the bottom and top values to get them into our coordinate system (0,0 = bottom left)>
     */
    return {{float(boundingRect.left()), float(-boundingRect.bottom()), 0.0},
            {float(boundingRect.right()), float(-boundingRect.top()), depth}};
}

// Return bounding cuboid with translation and anchor point applied
Cuboid TextEntity::boundingCuboid(const QFont &font, const QString &text, QVector3D anchorPosition,
                                  MildredMetrics::AnchorPoint anchorPoint, float depth)
{
    // Get basic bounding cuboid for the text
    auto cuboid = boundingCuboid(font, text, depth);

    // Get the anchor location and translate our coordinates so the anchor is at (0,0)
    auto anchorFrac = MildredMetrics::anchorLocation(anchorPoint);
    cuboid.translate(anchorPosition -
                     QVector3D(cuboid.xExtent() * anchorFrac.x(), cuboid.upperRightFront().y() * anchorFrac.y(), 0.0));

    return cuboid;
}
