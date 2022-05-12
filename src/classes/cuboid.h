#pragma once

#include <QVector3D>

namespace Mildred
{
//! Cuboid defines a region in 3D space
/*!
 * The @class Cuboid class defines a cuboid in 3D space from its lower-left-back and upper-right-front coordinates.
 */
class Cuboid
{
    public:
    Cuboid() = default;
    Cuboid(QVector3D v1, QVector3D v2);
    ~Cuboid() = default;

    private:
    // Extreme corners of cuboid at the lower left back and upper right front
    std::optional<double> v1x_, v1y_, v1z_, v2x_, v2y_, v2z_;

    public:
    // Reset the cuboid to null size
    void reset();
    // Set x extent
    void setXExtent(double extentMin, double extentMax);
    // Set y extent
    void setYExtent(double extentMin, double extentMax);
    // Set z extent
    void setZExtent(double extentMin, double extentMax);
    // Return the lower left back coordinate
    QVector3D lowerLeftBack() const;
    // Return the upper right front coordinate
    QVector3D upperRightFront() const;
    // Expand cuboid to encompass supplied (optional) coordinates
    void expand(std::optional<double> x, std::optional<double> y, std::optional<double> z);
    // Expand cuboid to encompass supplied point
    void expand(const QVector3D &v);
    // Expand cuboid to encompass supplied cuboid
    void expand(const Cuboid &cuboid);
    // Translate the cuboid by the supplied vector
    void translate(QVector3D v);
    // Expand x extent of cuboid by given amount
    void expandX(double delta);
    // Expand y extent of cuboid by given amount
    void expandY(double delta);
    // Expand z extent of cuboid by given amount
    void expandZ(double delta);
    // Return whether a valid extent in x exists
    bool validXExtent() const;
    // Return whether a valid extent in y exists
    bool validYExtent() const;
    // Return whether a valid extent in z exists
    bool validZExtent() const;
    // Return x extent of cuboid
    float xExtent() const;
    // Return y extent of cuboid
    float yExtent() const;
    // Return z extent of cuboid
    float zExtent() const;
    // Return vector of extents
    QVector3D extents() const;
};
} // namespace Mildred