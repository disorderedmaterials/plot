#pragma once

#include "entities/data.h"

namespace Mildred
{
//! DataRenderer2DBase is the base class for all 2-dimensional data renderers.
/*!
 * DataRenderer2DBase provides a base class for all 2-dimensional data rendering styles.
 */
class DataRenderer2D
{
    public:
    DataRenderer2D(Qt3DCore::QEntity *rootEntity) : rootEntity_(rootEntity){};
    virtual ~DataRenderer2D() = default;

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
