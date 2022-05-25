#pragma once

#include "entities/data.h"
#include "renderers/2d/stylefactory.h"

namespace Mildred
{
// Forward Declarations
class AxisEntity;
//! Data2DEntity encapsulates entities representing a 2-dimensional dataset.
/*!
 * Data2DEntity provides a general class to contain and display a single two-dimensional dataset.
 *
 * Data passed to the class in the form of three std::vector<double> containing x and y axis points and values. The vector sizes
 * of the axis values provided determines the expected size of the value points vector.
 */
class Data2DEntity : public DataEntity
{
    public:
    Data2DEntity(const AxisEntity *xAxis, const AxisEntity *yAxis, const AxisEntity *valueAxis, Qt3DCore::QNode *parent = nullptr,
                 StyleFactory2D::Style style = StyleFactory2D::Style::Line);
    ~Data2DEntity() = default;

    /*
     * Data
     */
    protected:
    // Axis values
    std::vector<double> x_, y_;
    // Data values
    std::vector<double> values_;
    // Errors
    std::vector<double> errors_;

    public:
    // Clear all data
    void clearData();
    // Set display data
    void setData(std::vector<double> x, std::vector<double> y, std::vector<double> values);

    /*
     * Rendering
     */
    private:
    // Axes to plot against
    const AxisEntity *xAxis_{nullptr}, *yAxis_{nullptr}, *valueAxis_{nullptr};
    // Data style
    StyleFactory2D::Style style_;
    // Data Renderer
    std::shared_ptr<DataRenderer2D> dataRenderer_{nullptr};
    // Orientation
    AxisEntity::AxisType abscissa_{AxisEntity::AxisType::Horizontal}, ordinate_{AxisEntity::AxisType::Vertical}, applicate_{AxisEntity::AxisType::Depth};

    protected:
    // Create renderables from current data
    void create() override;
};
} // namespace Mildred