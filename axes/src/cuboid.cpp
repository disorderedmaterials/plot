#include "cuboid.h"

Cuboid::Cuboid(QVector3D v1, QVector3D v2) : v1_(v1), v2_(v2) {}

// Reset the cuboid to zero size
void Cuboid::zero()
{
    v1_ = QVector3D(0.0, 0.0, 0.0);
    v2_ = QVector3D(0.0, 0.0, 0.0);
}

// Return the lower left back coordinate
QVector3D Cuboid::lowerLeftBack() const { return v1_; }
// Return the upper right front coordinate
QVector3D Cuboid::upperRightFront() const { return v2_; }

// Expand cuboid to encompass supplied cuboid
void Cuboid::expand(const Cuboid &cuboid)
{
    if (cuboid.lowerLeftBack().x() < v1_.x() || cuboid.upperRightFront().x() < v1_.x())
        v1_.setX(std::min(cuboid.lowerLeftBack().x(), cuboid.upperRightFront().x()));
    if (cuboid.lowerLeftBack().y() < v1_.y() || cuboid.upperRightFront().y() < v1_.y())
        v1_.setY(std::min(cuboid.lowerLeftBack().y(), cuboid.upperRightFront().y()));
    if (cuboid.lowerLeftBack().z() < v1_.z() || cuboid.upperRightFront().z() < v1_.z())
        v1_.setZ(std::min(cuboid.lowerLeftBack().z(), cuboid.upperRightFront().z()));

    if (cuboid.lowerLeftBack().x() > v2_.x() || cuboid.upperRightFront().x() > v2_.x())
        v2_.setX(std::max(cuboid.lowerLeftBack().x(), cuboid.upperRightFront().x()));
    if (cuboid.lowerLeftBack().y() > v2_.y() || cuboid.upperRightFront().y() > v2_.y())
        v2_.setY(std::max(cuboid.lowerLeftBack().y(), cuboid.upperRightFront().y()));
    if (cuboid.lowerLeftBack().z() > v2_.z() || cuboid.upperRightFront().z() > v2_.z())
        v2_.setZ(std::max(cuboid.lowerLeftBack().z(), cuboid.upperRightFront().z()));
}

// Translate the cuboid by the supplied vector
void Cuboid::translate(QVector3D v)
{
    v1_ += v;
    v2_ += v;
}

// Return x extent of cuboid
float Cuboid::xExtent() const { return v2_.x() - v1_.x(); }

// Return y extent of cuboid
float Cuboid::yExtent() const { return v2_.y() - v1_.y(); }

// Return z extent of cuboid
float Cuboid::zExtent() const { return v2_.z() - v1_.z(); }

// Return vector of extents
QVector3D Cuboid::extents() const { return {v2_.x() - v1_.x(), v2_.y() - v1_.y(), v2_.z() - v1_.z()}; }
