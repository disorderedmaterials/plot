#pragma once

#include "entities/line.h"
#include "renderers/1d/base.h"

namespace Mildred
{
//! TriangleSymbolRenderer1D renders 1D data symbols as a triangle.
/*!
 * TriangleSymbolRenderer1D manages the creation of entities for displaying 1D data as a simple triangle.
 */
class TriangleSymbolRenderer1D : public SymbolRenderer1D
    {
        public:
        TriangleSymbolRenderer1D(Qt3DCore::QEntity *rootEntity);
        ~TriangleSymbolRenderer1D();

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
