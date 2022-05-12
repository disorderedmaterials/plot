#pragma once

#include "entities/data.h"

namespace Mildred
{
//! DataRenderer1DBase is the base class for all 1-dimensional data renderers.
/*!
 * DataRenderer1DBase provides a base class for all 1-dimensional data rendering styles.
 */
class DataRenderer1D
{
    public:
    DataRenderer1D(Qt3DCore::QEntity *rootEntity) : rootEntity_(rootEntity){};
    virtual ~DataRenderer1D() = default;

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
                        const std::vector<double> &values, const AxisEntity *valueAxis) = 0;
};
} // namespace Mildred