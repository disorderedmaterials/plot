#pragma once

#include "entities/line.h"
#include "renderers/1d/base.h"

namespace Mildred
{
//! ErrorBarRenderer1D renders 1D data errors as sticks.
/*!
 * ErrorBarRenderer1D manages the creation of entities for displaying 1D data error bars in a simple stick representation.
 */
class ErrorBarRenderer1D : public ErrorRenderer1D
{
    public:
    ErrorBarRenderer1D(Qt3DCore::QEntity *rootEntity);
    ~ErrorBarRenderer1D();

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