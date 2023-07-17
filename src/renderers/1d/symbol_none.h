#pragma once

#include "renderers/1d/base.h"

namespace Mildred
{
//! NoSymbolRenderer1D renders no symbols.
/*!
 * NoSymbolRenderer1D is a dummy class to represent valid, but 'None' symbols.
 */
class NoSymbolRenderer1D : public SymbolRenderer1D
{
    public:
    NoSymbolRenderer1D(Qt3DCore::QEntity *rootEntity) : SymbolRenderer1D(rootEntity) {}
    ~NoSymbolRenderer1D(){};

    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                const std::vector<double> &values, const AxisEntity *valueAxis) override{};
};
} // namespace Mildred