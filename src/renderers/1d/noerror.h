#pragma once

#include "renderers/1d/base.h"

namespace Mildred
{
//! NoErrorRenderer1D renders no error bars.
/*!
 * NoErrorRenderer1D is a dummy class to represent valid, but 'None' error bars.
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