#pragma once

#include "entities/line.h"
#include "renderers/2d/base.h"

namespace Mildred
{
//! LineRenderer1D renders 2D data as a line.
/*!
 * LineRenderer2D manages the creation of entities for displaying 2D data as a simple line.
 */
class LineRenderer2D : public DataRenderer2D
{
    public:
    LineRenderer2D(Qt3DCore::QEntity *rootEntity);
    ~LineRenderer2D();

    /*
     * Rendering
     */
    private:
    // Line entity
    LineEntity *lines_{nullptr};
    // std::vector<LineEntity*> lines_;

    public:
    // Create entities from the supplied metrics and data
    void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                    const std::vector<double> &y, const AxisEntity *yAxis, const std::vector<double> &values, const AxisEntity *valueAxis);
};
} // namespace Mildred