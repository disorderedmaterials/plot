#pragma once

#include "entities/data.h"
#include "renderers/1d/stylefactory.h"

namespace Mildred
{
// Forward Declarations
class AxisEntity;

//! Data1DEntity encapsulates entities representing a 1-dimensional dataset.
/*!
 * Data1DEntity provides a general class to contain and display a single one-dimensional dataset.
 *
 * Data passed to the class in the form of two std::vector<double> containing x axis points and y axis values. The sizes of the
 * two arrays must match.
 */
class Data1DEntity : public DataEntity
{
    public:
    Data1DEntity(const AxisEntity *xAxis, const AxisEntity *valueAxis, Qt3DCore::QNode *parent = nullptr,
                 StyleFactory1D::Style style = StyleFactory1D::Style::Line,
                 StyleFactory1D::ErrorBarStyle errorStyle = StyleFactory1D::ErrorBarStyle::T_Stick);
    ~Data1DEntity() = default;

    /*
     * Data
     */
    protected:
    // Axis values
    std::vector<double> x_;
    // Data values
    std::vector<double> values_;
    // Error values
    std::vector<double> errors_;

    public:
    // Clear all data
    void clearData();
    // Set display data
    void setData(std::vector<double> x, std::vector<double> values, std::optional<std::vector<double>> errors = std::nullopt);

    /*
     * Rendering
     */
    private:
    // Axes to plot against
    const AxisEntity *xAxis_{nullptr}, *valueAxis_{nullptr};
    // Data style
    StyleFactory1D::Style style_;
    // Data Renderer
    std::shared_ptr<DataRenderer1D> dataRenderer_{nullptr};
    // Error style
    StyleFactory1D::ErrorBarStyle errorStyle_;
    // Error Renderer
    std::shared_ptr<ErrorRenderer1D> errorRenderer_{nullptr};
    // Orientation
    AxisEntity::AxisType abscissa_{AxisEntity::AxisType::Horizontal}, ordinate_{AxisEntity::AxisType::Vertical};

    public:
    // Set line style
    void setLineStyle(StyleFactory1D::Style style);
    // Set error style
    void setErrorStyle(StyleFactory1D::ErrorBarStyle style);
    // Set error size
    void setErrorBarSize(float size);
    // Get error size
    float errorBarSize() const;

    protected:
    // Create renderables from current data
    void create() override;
};
} // namespace Mildred