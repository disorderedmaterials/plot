#include "classes/cuboid.h"

using namespace Mildred;

//! Construct a new Cuboid from the supplied positions
Cuboid::Cuboid(QVector3D v1, QVector3D v2) : v1x_(v1.x()), v1y_(v1.y()), v1z_(v1.z()), v2x_(v2.x()), v2y_(v2.y()), v2z_(v2.z())
{
}

//! Reset the cuboid to null size
/*!
 * Set all extreme coordinates to null.
 */
void Cuboid::reset()
{
    v1x_ = std::nullopt;
    v1y_ = std::nullopt;
    v1z_ = std::nullopt;
    v2x_ = std::nullopt;
    v2y_ = std::nullopt;
    v2z_ = std::nullopt;
}

//! Set x extent
void Cuboid::setXExtent(double extentMin, double extentMax)
{
    v1x_ = extentMin;
    v2x_ = extentMax;
}

//! Set y extent
void Cuboid::setYExtent(double extentMin, double extentMax)
{
    v1y_ = extentMin;
    v2y_ = extentMax;
}

//! Set z extent
void Cuboid::setZExtent(double extentMin, double extentMax)
{
    v1z_ = extentMin;
    v2z_ = extentMax;
}

//! Return the lower left back coordinate
/*!
 * Return the lower left back coordinate of the cuboid as a QVector3D. Any as-yet undefined coordinates will be set to zero.
 */
QVector3D Cuboid::lowerLeftBack() const
{
    return {float(v1x_.value_or(0.0)), float(v1y_.value_or(0.0)), float(v1z_.value_or(0.0))};
}

//! Return the upper right front coordinate
/*!
 * Return the upper right front coordinate of the cuboid as a QVector3D. Any as-yet undefined coordinates will be set to zero.
 */
QVector3D Cuboid::upperRightFront() const
{
    return {float(v2x_.value_or(0.0)), float(v2y_.value_or(0.0)), float(v2z_.value_or(0.0))};
}

//! Expand cuboid to encompass supplied (optional) coordinates
void Cuboid::expand(std::optional<double> x, std::optional<double> y, std::optional<double> z)
{
    if (x)
    {
        if (!v1x_ || x < v1x_.value())
            v1x_ = x;
        if (!v2x_ || x > v2x_.value())
            v2x_ = x;
    }

    if (y)
    {
        if (!v1y_ || y < v1y_.value())
            v1y_ = y;
        if (!v2y_ || y > v2y_.value())
            v2y_ = y;
    }

    if (z)
    {
        if (!v1z_ || z < v1z_.value())
            v1z_ = z;
        if (!v2z_ || z > v2z_.value())
            v2z_ = z;
    }
}

//! Expand cuboid to encompass supplied point
/*!
 * Expand the current extrema so as to encompass the supplied point @param v.
 */
void Cuboid::expand(const QVector3D &v)
{
    if (!v1x_ || v.x() < v1x_.value())
        v1x_ = v.x();
    if (!v1y_ || v.y() < v1y_.value())
        v1y_ = v.y();
    if (!v1z_ || v.z() < v1z_.value())
        v1z_ = v.z();

    if (!v2x_ || v.x() > v2x_.value())
        v2x_ = v.x();
    if (!v2y_ || v.y() > v2y_.value())
        v2y_ = v.y();
    if (!v2z_ || v.z() > v2z_.value())
        v2z_ = v.z();
}

//! Expand cuboid to encompass supplied cuboid
/*!
 * Expand the current extrema so as to completely encompass the supplied @param cuboid.
 */
void Cuboid::expand(const Cuboid &cuboid)
{
    expand(cuboid.v1x_, cuboid.v1y_, cuboid.v1z_);
    expand(cuboid.v2x_, cuboid.v2y_, cuboid.v2z_);
}

//! Translate the cuboid by the supplied vector
void Cuboid::translate(QVector3D v)
{
    if (v1x_)
        *v1x_ += v.x();
    if (v1y_)
        *v1y_ += v.y();
    if (v1z_)
        *v1z_ += v.z();
    if (v2x_)
        *v2x_ += v.x();
    if (v2y_)
        *v2y_ += v.y();
    if (v2z_)
        *v2z_ += v.z();
}

//! Expand x extent of cuboid by given amount
void Cuboid::expandX(double delta)
{
    v1x_ = v1x_.value_or(0.0) - delta * 0.5;
    v2x_ = v2x_.value_or(0.0) + delta * 0.5;
}

//! Expand y extent of cuboid by given amount
void Cuboid::expandY(double delta)
{
    v1y_ = v1y_.value_or(0.0) - delta * 0.5;
    v2y_ = v2y_.value_or(0.0) + delta * 0.5;
}

//! Expand z extent of cuboid by given amount
void Cuboid::expandZ(double delta)
{
    v1z_ = v1z_.value_or(0.0) - delta * 0.5;
    v2z_ = v2z_.value_or(0.0) + delta * 0.5;
}

//! Return whether a valid extent in x exists
bool Cuboid::validXExtent() const { return v1x_ && v2x_; }

//! Return whether a valid extent in y exists
bool Cuboid::validYExtent() const { return v1y_ && v2y_; }

//! Return whether a valid extent in z exists
bool Cuboid::validZExtent() const { return v1z_ && v2z_; }

//! Return x extent of cuboid
float Cuboid::xExtent() const
{
    if (v1x_ && v2x_)
        return *v2x_ - *v1x_;
    else
        return 0.0;
}

//! Return y extent of cuboid
float Cuboid::yExtent() const
{
    if (v1y_ && v2y_)
        return *v2y_ - *v1y_;
    else
        return 0.0;
}

//! Return z extent of cuboid
float Cuboid::zExtent() const
{
    if (v1z_ && v2z_)
        return *v2z_ - *v1z_;
    else
        return 0.0;
}

//! Return vector of extents
QVector3D Cuboid::extents() const { return {xExtent(), yExtent(), zExtent()}; }
