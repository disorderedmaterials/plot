#include "entities/axis.h"

using namespace Mildred;

constexpr auto showBoundingBoxes = false;

//! Construct a new text entity
/*!
 * Constructs a new @class TextEntity with the supplied text and default transform.
 *
 * The entity leverages QExtrudedTextMesh in order to generate the 3D representation of the glyphs required. These are generate
 * to be unit height, so the scaling factors of the associated QTransform are set so as to reproduce the original point size in
 * pixels.
 */
TextEntity::TextEntity(Qt3DCore::QNode *parent, QString text) : Qt3DCore::QEntity(parent)
{
    // Create a positional transform for our collection of entities
    positionalTransform_ = new Qt3DCore::QTransform(this);
    addComponent(positionalTransform_);

    // Create the main text entity
    textEntity_ = new Qt3DCore::QEntity(this);

    // Create an extruded text mesh
    textMesh_ = new Qt3DExtras::QExtrudedTextMesh(textEntity_);
    textMesh_->setFont(QFont("monospace", 10.0));
    textMesh_->setText(text);
    textMesh_->setDepth(0.1);
    textEntity_->addComponent(textMesh_);

    // Create a local transform and scale the text from unit height to the point size
    textTransform_ = new Qt3DCore::QTransform(textMesh_);
    textTransform_->setScale3D({10.0f, 10.0f, 1.0f});
    textEntity_->addComponent(textTransform_);

    // Add bounding box entity
    if (showBoundingBoxes)
    {
        boundingBoxEntity_ = new LineEntity(this);
        boundingBoxTransform_ = new Qt3DCore::QTransform(boundingBoxEntity_);
        boundingBoxEntity_->addComponent(boundingBoxTransform_);
        anchorPointEntity_ = new LineEntity(this, Qt3DRender::QGeometryRenderer::PrimitiveType::Lines);
        anchorPointEntity_->addVertex({3.0, 0.0, 0.0});
        anchorPointEntity_->addVertex({-3.0, 0.0, 0.0});
        anchorPointEntity_->addVertex({0.0, 3.0, 0.0});
        anchorPointEntity_->addVertex({0.0, -3.0, 0.0});
        anchorPointEntity_->setBasicIndices();
        anchorPointEntity_->finalise();
    }
}

// Set text material
void TextEntity::setMaterial(RenderableMaterial *material)
{
    // Remove existing material if one exists
    if (material_)
        foreach (auto *node, childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->removeComponent(material_);
        }

    material_ = material;
    if (material_)
        foreach (auto *node, childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->addComponent(material_);
        }
}

/*
 * Definition
 */

//! Update translation
/*!
 * Update the translation such that the stored anchor point is located at the stored position in 3D space.
 */
void TextEntity::updateTranslation()
{
    // Set the main positional transform
    positionalTransform_->setTranslation(anchorPosition_);

    // Set the text translation vector so that the defined anchor point is located at {0,0,0}
    auto [textCuboid, descent] = boundingCuboid(textMesh_->font(), textMesh_->text(), {}, anchorPoint_, textMesh_->depth());
    auto v = QVector3D(textCuboid.lowerLeftBack().x(), textCuboid.lowerLeftBack().y(), 0.0);
    textTransform_->setTranslation(v);

    // Update the bounding box entity
    if (boundingBoxEntity_)
    {
        boundingBoxEntity_->clear();
        boundingBoxEntity_->addVertex({textCuboid.lowerLeftBack().x(), textCuboid.lowerLeftBack().y(), 0});
        boundingBoxEntity_->addVertex({textCuboid.upperRightFront().x(), textCuboid.lowerLeftBack().y(), 0});
        boundingBoxEntity_->addVertex({textCuboid.upperRightFront().x(), textCuboid.upperRightFront().y(), 0});
        boundingBoxEntity_->addVertex({textCuboid.lowerLeftBack().x(), textCuboid.upperRightFront().y(), 0});
        boundingBoxEntity_->addVertex({textCuboid.lowerLeftBack().x(), textCuboid.lowerLeftBack().y(), 0});
        boundingBoxEntity_->setBasicIndices();
        boundingBoxEntity_->finalise();
    }
}

//! Set text
void TextEntity::setText(const QString &text)
{
    textMesh_->setText(text);

    updateTranslation();
}

//! Return current text
QString TextEntity::text() const { return textMesh_->text(); }

//! Set font
void TextEntity::setFont(const QFont &font)
{
    textMesh_->setFont(font);

    // Set the scale factor to be consistent with the axisTickLabelFont point size
    textTransform_->setScale3D({float(font.pointSizeF()), float(font.pointSizeF()), 1.0});

    updateTranslation();
}

//! Set anchor point
void TextEntity::setAnchorPoint(MildredMetrics::AnchorPoint anchor)
{
    anchorPoint_ = anchor;

    updateTranslation();
}

// 1 Set anchor position
void TextEntity::setAnchorPosition(QVector3D p)
{
    anchorPosition_ = p;

    updateTranslation();
}

//! Return simple bounding cuboid for text, along with baseline descent from font metrics
/*!
 * Calculates a bounding cuboid in the XY plane for the specified @param font and @param text. The @param depth is applied in
 * the z-direction, and the bounding cuboid's lower-left-back corner is located at (0,0,0).
 */
std::pair<Cuboid, int> TextEntity::boundingCuboid(const QFont &font, const QString &text, float depth)
{
    QFontMetrics fontMetrics(font);
    const auto boundingRect = fontMetrics.boundingRect(text);
    /*
     * Take the values out of boundingRect (which refers to Qt's coordinate system where 0,0 is top left) so we understand what
     * we're doing. We need to negate the bottom and top values to get them into our coordinate system (0,0 = bottom left).
     */
    return {{{float(boundingRect.left()), float(-boundingRect.bottom()), 0.0},
             {float(boundingRect.right()), float(-boundingRect.top()), depth}},
            fontMetrics.descent()};
}

//! Return bounding cuboid with translation and anchor point applied
/*!
 * Calculates a bounding cuboid in the XY plane for the specified @param font and @param text, and translated to put the
 * requested @param anchorPoint at the supplied @param anchorPosition. The @param depth is applied in the z-direction.
 */
std::pair<Cuboid, int> TextEntity::boundingCuboid(const QFont &font, const QString &text, QVector3D anchorPosition,
                                                  MildredMetrics::AnchorPoint anchorPoint, float depth)
{
    // Get basic bounding cuboid for the text
    auto [cuboid, descent] = boundingCuboid(font, text, depth);

    /*
     * Get the anchor location and translate our coordinates so the anchor is at (0,0)
     * Correct for negative lower y values, occurring from QFontMetrics returning a negative lower y limit which represents
     * the descent of characters below the baseline.
     *
     * For horizontal anchors, remove it twice to better centre the letter centres with the anchor point.
     */
    auto anchorFrac = MildredMetrics::anchorLocation(anchorPoint);
    auto bottomEdgeDrop = cuboid.lowerLeftBack().y();

    // Translate the cuboid according to the anchor point, and remove any bottom "edge" drop arising from the letter baseline
    // descent.
    cuboid.translate(anchorPosition -
                     QVector3D(cuboid.xExtent() * anchorFrac.x(), cuboid.yExtent() * anchorFrac.y() - descent, 0.0));

    // For primarily vertical anchor points, add on the bottom edge drop again to better locate the text relative to the anchor
    // point
    if (MildredMetrics::isAnchorPointVertical(anchorPoint))
        cuboid.translate({0.0, -bottomEdgeDrop, 0.0});

    return {cuboid, descent};
}
