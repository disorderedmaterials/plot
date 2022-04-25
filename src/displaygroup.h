#pragma once

#include "entities/data.h"
#include <QColor>

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

    /*
     * Update
     */
    private:
    // Apply colours and transforms to all targeted entities
    void apply();
};
} // namespace Mildred
