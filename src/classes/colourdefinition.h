#pragma once

#include <QColor>
#include <vector>

namespace Mildred
{
//! ColourDefinition completely defines colour variance over a range of values.
class ColourDefinition
{
    public:
    ColourDefinition() = default;
    explicit ColourDefinition(QColor colour);
    ColourDefinition(const ColourDefinition &);
    void operator=(const ColourDefinition &source);
    void clear();

    /*
     * Points
     */
    private:
    // Point delta typedef
    using PointDelta = std::tuple<double, double, std::array<double, 4>, std::array<double, 4>>;
    // Vector of value/colour points
    std::vector<std::pair<double, QColor>> points_;
    // Vector of delta between points
    std::vector<PointDelta> deltas_;

    private:
    // Calculate deltas from current points
    void calculateDeltas();

    public:
    // Add new point
    void addPoint(double value, QColor colour);
    // Set simple, single colour
    void set(QColor colour);
    // Get colour associated with value supplied
    QColor colour(double value) const;

    /*
     * Style
     */
    private:
    // Whether colours are interpolated between points in the scale
    bool interpolated_{true};
    // Whether interpolation should be performed over HSV instead of RGB
    bool useHSV_{false};

    public:
    // Set whether the ColourDefinition is interpolated
    void setInterpolated(bool b);
    // Return whether the ColourDefinition is interpolated
    bool interpolated() const;
    // Set whether or not to use HSV interpolation
    void setUseHSV(bool b);
    // Return whether or not to use HSV interpolation
    bool useHSV() const;
};
} // namespace Mildred