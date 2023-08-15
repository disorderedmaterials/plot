#pragma once

#include "entities/line.h"
#include "renderers/3d/base.h"

namespace Mildred
{
//! LineRenderer3D renders 3D data as a line.
/*!
 * LineRenderer3D manages the creation of entities for displaying 3D data as a simple line.
 */
class LineRenderer3D : public DataRenderer3D
{
    public:
    LineRenderer3D(Qt3DCore::QEntity *rootEntity);
    ~LineRenderer3D();

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
