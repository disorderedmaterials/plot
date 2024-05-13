#pragma once

#include <optional>
#include <vector>

namespace Mildred
{
//! Data1DSource provides access to 1D data for display.
/*!
 * Data1DSource acts as a container / interface to one-dimensional data to be displayed on a MildredWidget.
 * TODO
 */
class Data1DSource
{
    public:
    Data1DSource() = default;
    ~Data1DSource() = default;

    private:
    // Whether the data is locally stored or must be retrieved
    bool local_{true};
    // Version counter for data
    int version_{0};
    // Axis values
    std::vector<double> x_;
    // Data values
    std::vector<double> values_;
    // Error values
    std::vector<double> errors_;

    public:
    // Set local data
    void setData(std::vector<double> x, std::vector<double> values, std::optional<std::vector<double>> errors = std::nullopt);
};
} // namespace Mildred
