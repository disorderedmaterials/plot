#pragma once

#include "entities/data.h"

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
    public:
    // Create entities from the supplied axes and data
    virtual Cuboid create(const std::vector<double> &x, const AxisEntity *xAxis, const std::vector<double> &values,
                          const AxisEntity *valueAxis) = 0;
    // Recreate entities from stored
    Cuboid recreate();
};
