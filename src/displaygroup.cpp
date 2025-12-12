#include "displaygroup.h"
#include <QPainter>

using namespace Mildred;

/*
 * Targets
 */

// Add target data entity to group
void DisplayGroup::addTarget(Mildred::DataEntity *entity)
{
    auto it = std::find(targets_.begin(), targets_.end(), entity);
    if (it != targets_.end())
        return;

    targets_.emplace_back(entity);

    applyToEntity(entity, targets_.size() - 1);
}

// Remove data entity from group
void DisplayGroup::removeTarget(DataEntity *entity)
{
    auto it = std::remove(targets_.begin(), targets_.end(), entity);
    if (it != targets_.end())
        targets_.erase(it);

    // TODO update all entities
}

/*
 * Colouring
 */

// Stock Colours
QColor StockColours[] = {
    QColor(0, 0, 0),       /* BlackStockColour */
    QColor(200, 0, 0),     /* RedStockColour */
    QColor(0, 200, 0),     /* GreenStockColour */
    QColor(0, 0, 200),     /* BlueStockColour */
    QColor(200, 0, 200),   /* PurpleStockColour */
    QColor(0, 200, 200),   /* CyanStockColour */
    QColor(255, 128, 0),   /* OrangeStockColour */
    QColor(255, 100, 255), /* PinkStockColour */
    QColor(255, 100, 100), /* LightRedStockColour */
    QColor(100, 255, 100), /* LightGreenStockColour */
    QColor(100, 100, 255), /* LightBlueStockColour */
    QColor(128, 128, 128), /* SilverStockColour */
    QColor(128, 128, 50)   /* GoldStockColour */
};

// Set colour policy for the group
void DisplayGroup::setColourPolicy(DisplayGroup::ColourPolicy policy)
{
    if (colourPolicy_ == policy)
        return;

    colourPolicy_ = policy;

    apply();
}

// Return colour policy for the group
DisplayGroup::ColourPolicy DisplayGroup::colourPolicy() const { return colourPolicy_; }

// Set single colour
void DisplayGroup::setSingleColour(QColor colour)
{
    singleColour_ = colour;

    if (colourPolicy_ == ColourPolicy::Single)
        apply();
}

// Return single colour
QColor DisplayGroup::singleColour() const { return singleColour_; }

// Set stock colour
void DisplayGroup::setStockColour(StockColour colour)
{
    stockColour_ = colour;

    if (colourPolicy_ == ColourPolicy::Stock)
        apply();
}

// Return stock colour
DisplayGroup::StockColour DisplayGroup::stockColour() const { return stockColour_; }

// Return suitable stock colour for the integer index supplied
DisplayGroup::StockColour DisplayGroup::stockColourForIndex(int index) { return (StockColour)(index % nStockColours); }

// Set gradient to apply
void DisplayGroup::setGradient(const ColourDefinition &gradient)
{
    gradient_ = gradient;

    if (colourPolicy_ == ColourPolicy::Gradient)
        apply();
}

// Return gradient to apply
const ColourDefinition &DisplayGroup::gradient() const { return gradient_; }

// Return icon representing current colour policy
QIcon DisplayGroup::colourPolicyIcon(QSize size)
{
    QPixmap pixmap(size);
    QRect rect(0, 0, size.width(), size.height());
    QPainter painter(&pixmap);

    switch (colourPolicy_)
    {
        case (ColourPolicy::None):
            painter.setPen(QColor(0.0, 0.0, 0.0, 1.0));
            painter.drawRect(rect);
            painter.drawLine(0, 0, size.width(), size.height());
            painter.drawLine(0, size.height(), size.width(), 0);
            break;
        case (ColourPolicy::Stock):
            painter.fillRect(rect, StockColours[stockColour_]);
            break;
        case (ColourPolicy::Single):
            painter.fillRect(rect, singleColour_);
            break;
        case (ColourPolicy::Varying):
            // TODO
            break;
        case (ColourPolicy::Gradient):
            // TODO
            break;
    }

    painter.end();
    return {pixmap};
}

/*
 * Data Translation
 */

// Set translation policy for the X axis
void DisplayGroup::setTranslationPolicyX(DisplayGroup::TranslationPolicy policy)
{
    if (translationPolicyX_ == policy)
        return;

    translationPolicyX_ = policy;

    apply();
}

// Return translation policy for the X axis
DisplayGroup::TranslationPolicy DisplayGroup::translationPolicyX() const { return translationPolicyX_; }

// Set translation value for X axis
void DisplayGroup::setTranslationX(float translation)
{
    translationX_ = translation;

    apply();
}

// Return translation value for X axis
float DisplayGroup::translationX() const { return translationX_; }

// Set translation policy for the Y axis
void DisplayGroup::setTranslationPolicyY(DisplayGroup::TranslationPolicy policy)
{
    if (translationPolicyY_ == policy)
        return;

    translationPolicyY_ = policy;

    apply();
}

// Return translation policy for the Y axis
DisplayGroup::TranslationPolicy DisplayGroup::translationPolYcyX() const { return translationPolicyY_; }

// Set translation value for Y axis
void DisplayGroup::setTranslationY(float translation)
{
    translationY_ = translation;

    apply();
}

// Return translation value for Y axis
float DisplayGroup::translationY() const { return translationY_; }

/*
 * Update
 */

// Apply colours and transforms to specified entity
void DisplayGroup::applyToEntity(DataEntity *entity, int groupIndex)
{
    // Colour
    switch (colourPolicy_)
    {
        case (ColourPolicy::None):
            entity->removeColourOverride();
            break;
        case (ColourPolicy::Stock):
            entity->setColourOverride(ColourDefinition(StockColours[stockColour_]));
            break;
        case (ColourPolicy::Single):
            entity->setColourOverride(ColourDefinition(singleColour_));
            break;
        case (ColourPolicy::Varying):
            entity->setColourOverride(ColourDefinition(StockColours[groupIndex % nStockColours]));
            break;
        case (ColourPolicy::Gradient):
            entity->setColourOverride(gradient_);
            break;
    }

    // Translation
    QVector3D translation;
    switch (translationPolicyX_)
    {
        case (TranslationPolicy::None):
            break;
        case (TranslationPolicy::Constant):
            translation.setX(translationX_);
            break;
        case (TranslationPolicy::Incremental):
            translation.setX(translationX_ * groupIndex);
            break;
    }
    switch (translationPolicyY_)
    {
        case (TranslationPolicy::None):
            break;
        case (TranslationPolicy::Constant):
            translation.setX(translationY_);
            break;
        case (TranslationPolicy::Incremental):
            translation.setX(translationY_ * groupIndex);
            break;
    }
    entity->setPositionalTranslation(translation);
}

// Apply colours and transforms to all targeted entities
void DisplayGroup::apply()
{
    auto index = 0;
    for (auto &entity : targets_)
        applyToEntity(entity, index++);
}
