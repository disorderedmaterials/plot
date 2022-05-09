#include "colourdefinition.h"
#include <algorithm>

using namespace Mildred;

ColourDefinition::ColourDefinition(QColor colour) { set(colour); }

ColourDefinition::ColourDefinition(const ColourDefinition &source) { (*this) = source; }

void ColourDefinition::operator=(const ColourDefinition &source)
{
    clear();

    points_ = source.points_;
    deltas_ = source.deltas_;
    useHSV_ = source.useHSV_;
    interpolated_ = source.interpolated_;
}

// Clear ColourDefinition
void ColourDefinition::clear()
{
    points_.clear();
    deltas_.clear();
}

/*
 * Points
 */

// Add point to scale
void ColourDefinition::addPoint(double value, QColor colour)
{
    points_.emplace_back(value, colour);

    // Sort the points
    std::sort(points_.begin(), points_.end(), [value](const auto &p1, const auto &p2) { return p1.first < p2.first; });

    // Calculate deltas
    calculateDeltas();
}

// Set simple, single colour
void ColourDefinition::set(QColor colour)
{
    clear();

    points_.emplace_back(0.0, colour);
}

// Calculate deltas from current points
void ColourDefinition::calculateDeltas()
{
    deltas_.clear();

    for (auto n = 1; n < points_.size(); ++n)
    {
        const auto &p1 = points_[n - 1];
        const auto &p2 = points_[n];

        // Set the colour delta
        if (useHSV_)
            deltas_.emplace_back(
                PointDelta{p1.first,
                           p2.first,
                           {p1.second.hueF(), p1.second.saturationF(), p1.second.valueF(), p1.second.alphaF()},
                           {p2.second.hueF() - p1.second.hueF(), p2.second.saturationF() - p1.second.saturationF(),
                            p2.second.valueF() - p1.second.valueF(), p2.second.alphaF() - p1.second.alphaF()}});
        else
            deltas_.emplace_back(PointDelta{p1.first,
                                            p2.first,
                                            {p1.second.redF(), p1.second.greenF(), p1.second.blueF(), p1.second.alphaF()},
                                            {p2.second.redF() - p1.second.redF(), p2.second.greenF() - p1.second.greenF(),
                                             p2.second.blueF() - p1.second.blueF(), p2.second.alphaF() - p1.second.alphaF()}});
    }
}

// Return colour associated with value provided
QColor ColourDefinition::colour(double value) const
{
    // Check for no points being defined
    if (points_.empty())
        return {0, 0, 0};

    // Is supplied value less than the value at the first point?
    if (value <= points_.front().first)
        return points_.front().second;
    else if (value >= points_.back().first)
        return points_.back().second;

    // Find the correct delta to use
    auto it = std::find_if(deltas_.begin(), deltas_.end(),
                           [value](const auto &delta) { return std::get<0>(delta) <= value && std::get<1>(delta) > value; });
    if (it != deltas_.end())
    {
        const auto valueStart = std::get<0>(*it);
        const auto valueEnd = std::get<1>(*it);
        const auto &colourStart = std::get<2>(*it);
        const auto &colourDelta = std::get<3>(*it);

        if (interpolated_)
        {
            // Clamp value to range 0.0 - 1.0 to span range of delta
            auto clamped = (value - valueStart) / (valueEnd - valueStart);
            if (clamped < 0.0)
                clamped = 0.0;
            else if (clamped > 1.0)
                clamped = 1.0;

            if (useHSV_)
                return QColor::fromHsvF(colourStart[0] + colourDelta[0] * clamped, colourStart[1] + colourDelta[1] * clamped,
                                        colourStart[2] + colourDelta[2] * clamped, colourStart[3] + colourDelta[3] * clamped);
            else
                return QColor::fromRgbF(colourStart[0] + colourDelta[0] * clamped, colourStart[1] + colourDelta[1] * clamped,
                                        colourStart[2] + colourDelta[2] * clamped, colourStart[3] + colourDelta[3] * clamped);
        }
        else
            return useHSV_ ? QColor::fromHsvF(colourStart[0], colourStart[1], colourStart[2], colourStart[3])
                           : QColor::fromRgbF(colourStart[0], colourStart[1], colourStart[2], colourStart[3]);
    }

    printf("Failed to find a delta for value = %f\n", value);
    throw(std::runtime_error("Failed to find a colour to return from the ColourDefinition.\n"));
}

/*
 * Style
 */

// Set whether the ColourDefinition is interpolated
void ColourDefinition::setInterpolated(bool b) { interpolated_ = b; }

// Return whether the ColourDefinition is interpolated
bool ColourDefinition::interpolated() const { return interpolated_; }

// Set whether or not to use HSV interpolation
void ColourDefinition::setUseHSV(bool b)
{
    useHSV_ = b;

    calculateDeltas();
}

// Return whether or not to use HSV interpolation
bool ColourDefinition::useHSV() const { return useHSV_; }
