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

//! ErrorRenderer1DBase is the base class for all 1-dimensional error data renderers.
/*!
 * ErrorRenderer1DBase provides a base class for all 1-dimensional error data rendering styles.
 */
class ErrorRenderer1D
{
    public:
    ErrorRenderer1D(Qt3DCore::QEntity *rootEntity) : rootEntity_(rootEntity){};
    virtual ~ErrorRenderer1D() = default;

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

    // Error bar metric1
    double errorBarMetric_{10.0};

    public:
    // Create entities from the supplied axes and data
    virtual void create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                        const std::vector<double> &values, const std::vector<double> &errors, const AxisEntity *valueAxis) = 0;
    // Get error bar metric.
    double errorBarMetric() const;
    // Set error bar metric.
    void setErrorBarMetric(double errorBarMetric);
};

} // namespace Mildred