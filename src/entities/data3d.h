#pragma once

#include "entities/data.h"

namespace Mildred
{
//! Data2DEntity encapsulates entities representing a 3-dimensional dataset.
/*!
 * Data2DEntity provides a general class to contain and display a single two-dimensional dataset.
 *
 * Data passed to the class in the form of four std::vector<double> containing x, y, and z axis points and values. The vector
 * sizes of the axis values provided determines the expected size of the value points vector.
 */
class Data3DEntity : public DataEntity
{
    Q_OBJECT

    public:
    Data3DEntity(MildredMetrics &metrics, Qt3DCore::QNode *parent = nullptr);
    ~Data3DEntity() = default;

    /*
     * Data
     */
    private:
    // Axis values
    std::vector<double> x_, y_, z_;
    // Data values and errors
    std::vector<double> values_, errors_;

    public:
    // Clear all data
    void clearData();
    // Set display data
    void setData(std::vector<double> x, std::vector<double> y, std::vector<double> z, std::vector<double> values);
};
} // namespace Mildred