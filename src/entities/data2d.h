#pragma once

#include "entities/data.h"

//! Data2DEntity encapsulates entities representing a 2-dimensional dataset.
/*!
 * Data2DEntity provides a general class to contain and display a single two-dimensional dataset.
 *
 * Data passed to the class in the form of three std::vector<double> containing x and y axis points and values. The vector sizes
 * of the axis values provided determines the expected size of the value points vector.
 */
class Data2DEntity : public DataEntity
{
    Q_OBJECT

    public:
    Data2DEntity(MildredMetrics &metrics, Qt3DCore::QNode *parent = nullptr);
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
};
