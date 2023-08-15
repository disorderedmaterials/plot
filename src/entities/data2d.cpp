#include "entities/data2d.h"
#include "renderers/2d/stylefactory.h"

using namespace Mildred;

//! Create a new Data2DEntity
/*!
 * Construct a new Data2DEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
Data2DEntity::Data2DEntity(const AxisEntity *xAxis, const AxisEntity* yAxis, const AxisEntity *valueAxis, Qt3DCore::QNode *parent,
                           StyleFactory2D::Style style)
    : DataEntity(parent), xAxis_(xAxis), yAxis_(yAxis), valueAxis_(valueAxis), style_(style)
{
    dataRenderer_ = StyleFactory2D::createDataRenderer(style_, dataEntity_);
}
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

    if (x.size() * y.size() != values.size())
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (values)) so data will be ignored.\n", x.size(), y.size(), values.size());
    else
    {
        x_ = std::move(x);
        y_ = std::move(y);
        values_ = std::move(values);
    }

    // Determine axis extrema
    auto vit = values_.cbegin();
    for (const auto x : x_)
        for (const auto y : y_)
        {
        updateExtrema(x, y, *vit);
        ++vit;
    }

    create();
}

/*
 * Rendering
 */

//! Create renderables in the current style
void Data2DEntity::create()
{
    assert(dataRenderer_);
    dataRenderer_->create(colourDefinition(), x_, xAxis_, y_, yAxis_, values_, valueAxis_);
}
