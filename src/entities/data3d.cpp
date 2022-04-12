#include "entities/data3d.h"

using namespace Mildred;

//! Create a new Data3DEntity
/*!
 * Construct a new Data3DEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
Data3DEntity::Data3DEntity(MildredMetrics &metrics, Qt3DCore::QNode *parent) : DataEntity(parent) {}

/*
 * Data
 */

//! Clear all data vectors
void Data3DEntity::clearData()
{
    x_.clear();
    y_.clear();
    z_.clear();
    values_.clear();
    errors_.clear();
    extrema_ = Cuboid();
}

//! Set display data
/*!
 * Set the supplied three-dimensional data (axis points @param x, @param y, and @param z and @param values at those points). The
 * data are copied to local arrays and entities representing the data in the current style are immediately created.
 */
void Data3DEntity::setData(std::vector<double> x, std::vector<double> y, std::vector<double> z, std::vector<double> values)
{
    clearData();

    x_ = std::move(x);
    y_ = std::move(y);
    z_ = std::move(z);
    values_ = std::move(values);

    //    create();
}
