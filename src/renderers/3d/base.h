#pragma once

#include "entities/data.h"

namespace Mildred
{
//! DataRenderer3DBase is the base class for all 3-dimensional data renderers.
/*!
 * DataRenderer3DBase provides a base class for all 3-dimensional data rendering styles.
 */
class DataRenderer3D
{
    public:
    DataRenderer3D(Qt3DCore::QEntity *rootEntity) : rootEntity_(rootEntity){};
    virtual ~DataRenderer3D() = default;

    /*
     * Entities
     */
    protected:
    // Root entity
    Qt3DCore::QEntity *rootEntity_{nullptr};

    /*
     * Rendering
     */
    protected:
    // Colour definition
    ColourDefinition colour_;

    public:
    // Create entities from the supplied axes and data
    virtual void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                        const std::vector<double> &y, const AxisEntity *yAxis, const std::vector<double> &values, const AxisEntity *valueAxis) = 0;
};
}
