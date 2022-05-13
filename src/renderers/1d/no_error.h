#pragma once

#include "renderers/1d/base.h"
#include "renderers/1d/no_error.h"

namespace Mildred
{
//! LineRenderer1D renders 1D data as a line.
/*!
 * LineRenderer1D manages the creation of entities for displaying 1D data as a simple line.
 */
class NoErrorRenderer1D : public ErrorRenderer1D
{
    public:
    NoErrorRenderer1D(Qt3DCore::QEntity *rootEntity) : ErrorRenderer1D(rootEntity) {}
    ~NoErrorRenderer1D() {};
    
    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                  const std::vector<double> &values, const std::vector<double> &errors, const AxisEntity *valueAxis) override {};
};
} // namespace Mildred