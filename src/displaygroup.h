#pragma once

#include "entities/data.h"
#include <QColor>
#include <QIcon>

namespace Mildred
{
//! The DisplayGroup class contains colouring and transform policies and definitions.
/*!
 *
 */
class DisplayGroup
{
    /*
     * Targets
     */
    private:
    // Target data entities
    std::vector<DataEntity *> targets_;

    public:
    // Add target data entity to group
    void addTarget(DataEntity *entity);
    // Remove data entity from group
    void removeTarget(DataEntity *entity);

    /*
     * Colouring
     */
    public:
    // Colour Policy
    enum class ColourPolicy
    {
        None,
        Stock,
        Single,
        Varying,
        Gradient
    };
    // Stock Colours
    enum StockColour
    {
        Black,
        Red,
        Green,
        Blue,
        Purple,
        Cyan,
        Orange,
        Pink,
        LightRed,
        LightGreen,
        LightBlue,
        Silver,
        Gold,
        nStockColours
    };

    private:
    // Colour policy set for the group
    ColourPolicy colourPolicy_{ColourPolicy::None};
    // Colour to apply, if ColourPolicy == Single
    QColor singleColour_{Qt::black};
    // Stock colour to apply, if ColourPolicy == Stock
    StockColour stockColour_{Black};
    // Gradient to apply, if ColourPolicy == Gradient
    ColourDefinition gradient_;

    public:
    // Set colour policy for the group
    void setColourPolicy(ColourPolicy policy);
    // Return colour policy for the group
    ColourPolicy colourPolicy() const;
    // Set single colour
    void setSingleColour(QColor colour);
    // Return single colour
    QColor singleColour() const;
    // Set stock colour
    void setStockColour(StockColour colour);
    // Return stock colour
    StockColour stockColour() const;
    // Return suitable stock colour for the integer index supplied
    static StockColour stockColourForIndex(int index);
    // Set gradient to apply
    void setGradient(const ColourDefinition &gradient);
    // Return gradient to apply
    const ColourDefinition &gradient() const;
    // Return icon representing current colour policy
    QIcon colourPolicyIcon(QSize size);

    /*
     * Data Translation
     */
    public:
    // Translation Policy
    enum class TranslationPolicy
    {
        None,
        Constant,
        Incremental
    };

    private:
    // Translation policy for the x and y axes
    TranslationPolicy translationPolicyX_{TranslationPolicy::None}, translationPolicyY_{TranslationPolicy::None};
    // Translation values for x and y axes
    float translationX_{0.0}, translationY_{0.0};

    public:
    // Set translation policy for the X axis
    void setTranslationPolicyX(TranslationPolicy policy);
    // Return translation policy for the X axis
    TranslationPolicy translationPolicyX() const;
    // Set translation value for X axis
    void setTranslationX(float translation);
    // Return translation value for X axis
    float translationX() const;
    // Set translation policy for the Y axis
    void setTranslationPolicyY(TranslationPolicy policy);
    // Return translation policy for the Y axis
    TranslationPolicy translationPolYcyX() const;
    // Set translation value for Y axis
    void setTranslationY(float translation);
    // Return translation value for Y axis
    float translationY() const;

    /*
     * Update
     */
    private:
    // Apply colours and transforms to specified entity
    void applyToEntity(DataEntity *entity, int groupIndex);
    // Apply colours and transforms to all targeted entities
    void apply();
};
} // namespace Mildred
