#pragma once

#include "entities/line.h"
#include "renderers/1d/base.h"

namespace Mildred
{
//! SquareSymbolRenderer1D renders 1D data symbols as a square.
/*!
 * SquareSymbolRenderer1D manages the creation of entities for displaying 1D data as a simple square.
 */
class SquareSymbolRenderer1D : public SymbolRenderer1D
{
    public:
    SquareSymbolRenderer1D(Qt3DCore::QEntity *rootEntity);
    ~SquareSymbolRenderer1D();

    /*
     * Rendering
     */
    private:
    // Line entity
    LineEntity *symbols_{nullptr};

    public:
    // Create entities from the supplied metrics and data
    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                const std::vector<double> &values, const AxisEntity *valueAxis) override;
};

} // namespace Mildred
