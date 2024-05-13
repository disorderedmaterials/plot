#pragma once

#include <QString>
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
    Data1DSource(const QString &displayName = QString());
    ~Data1DSource() = default;

    /*
     * Basic Information
     */
    private:
    // Whether the data is locally stored or must be retrieved
    bool local_{true};
    // Version counter for data
    std::optional<int> version_;
    // Display name
    QString displayName_;

    public:
    // Return display name
    const QString &displayName() const;

    /*
     * Local Data
     */
    private:
    // Axis values
    std::vector<double> x_;
    // Data values
    std::vector<double> values_;
    // Error values
    std::vector<double> errors_;

    public:
    // Set local data
    void setData(std::vector<double> x, std::vector<double> values, std::optional<std::vector<double>> errors = std::nullopt);
    // Return axis values
    const std::vector<double> &x() const;
    // Return data values
    const std::vector<double> &values() const;
    // Return error values
    const std::vector<double> &errors() const;

    /*
     * External Data
     */
    private:
    // TODO

    /*
     * Update
     */
    public:
    // Update from specified source data if required
    bool updateFrom(const Data1DSource &source);
};
} // namespace Mildred
