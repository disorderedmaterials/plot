#include "entities/data2d.h"

using namespace Mildred;

//! Create a new Data2DEntity
/*!
 * Construct a new Data2DEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
Data2DEntity::Data2DEntity(MildredMetrics &metrics, Qt3DCore::QNode *parent) : DataEntity(parent) {}

/*
 * Data
 */

//! Clear all data vectors
void Data2DEntity::clearData()
{
    x_.clear();
    y_.clear();
    values_.clear();
    errors_.clear();
    extrema_ = Cuboid();
}

//! Set display data
/*!
 * Set the supplied two-dimensional data (axis points @param x and @param y and @param values at those points). The data are
 * copied to local arrays and entities representing the data in the current style are immediately created.
 */
void Data2DEntity::setData(std::vector<double> x, std::vector<double> y, std::vector<double> values)
{
    clearData();

    x_ = std::move(x);
    y_ = std::move(y);
    values_ = std::move(values);

    //    create();
}
