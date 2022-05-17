#pragma once

#include "entities/line.h"
#include "renderers/1d/base.h"

namespace Mildred
{
//! StickErrorRenderer1D renders 1D data errors as sticks.
/*!
 * StickErrorRenderer1D manages the creation of entities for displaying 1D data error bars in a simple stick representation.
 */
class StickErrorRenderer1D : public ErrorRenderer1D
{
    public:
    StickErrorRenderer1D(Qt3DCore::QEntity *rootEntity);
    ~StickErrorRenderer1D();

    /*
     * Rendering
     */
    private:
    // Line entity
    LineEntity *errors_{nullptr};

    public:
    // Create entities from the supplied metrics and data
    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                const std::vector<double> &values, const std::vector<double> &errors, const AxisEntity *valueAxis) override;
};
} // namespace Mildred