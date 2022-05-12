#pragma once

#include "entities/line.h"
#include "renderers/1d/base.h"

namespace Mildred
{
//! LineRenderer1D renders 1D data as a line.
/*!
 * LineRenderer1D manages the creation of entities for displaying 1D data as a simple line.
 */
class LineRenderer1D : public DataRenderer1D
{
    public:
    LineRenderer1D(Qt3DCore::QEntity *rootEntity);
    ~LineRenderer1D();

    /*
     * Rendering
     */
    private:
    // Line entity
    LineEntity *lines_{nullptr};

    public:
    // Create entities from the supplied metrics and data
    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                const std::vector<double> &values, const AxisEntity *valueAxis) override;
};
} // namespace Mildred