#pragma once

#include "renderers/1d/base.h"

namespace Mildred
{
//! NoLineRenderer1D renders no lines.
/*!
 * NoLineRenderer1D is a dummy class to represent valid, but 'None' lines.
 */
class NoLineRenderer1D : public DataRenderer1D
{
    public:
    NoLineRenderer1D(Qt3DCore::QEntity *rootEntity) : DataRenderer1D(rootEntity) {}
    ~NoLineRenderer1D() {};
    
    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                  const std::vector<double> &values, const AxisEntity *valueAxis) override {};
};
} // namespace Mildred