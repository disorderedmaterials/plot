#include "entities/data1d.h"
#include "renderers/1d/stylefactory.h"

using namespace Mildred;

//! Create a new Data1DEntity
/*!
 * Construct a new Data1DEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
Data1DEntity::Data1DEntity(const AxisEntity *xAxis, const AxisEntity *valueAxis, Qt3DCore::QNode *parent)
    : DataEntity(parent), xAxis_(xAxis), valueAxis_(valueAxis)
{
    dataRenderer_ = StyleFactory1D::createDataRenderer(style_, dataEntity_);
}

/*
 * Data
 */

//! Clear all data vectors
void Data1DEntity::clearData()
{
    x_.clear();
    values_.clear();
    errors_.clear();
    extrema_.zero();
}

//! Set display data (1D)
/*!
 * Set the supplied one-dimensional data (axis points @param x and @param values at those points). The data are copied to local
 * arrays and entities representing the data in the current style are immediately created.
 */
void Data1DEntity::setData(std::vector<double> x, std::vector<double> values)
{
    clearData();

    x_ = std::move(x);
    values_ = std::move(values);

    create();
}

/*
 * Rendering
 */

//! Create renderables in the current style
void Data1DEntity::create()
{
    assert(dataRenderer_);
    extrema_ = dataRenderer_->create(x_, xAxis_, values_, valueAxis_);
}
