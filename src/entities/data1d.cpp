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
    errorRenderer_ = StyleFactory1D::createErrorRenderer(errorStyle_, errorEntity_);
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
    extrema_.reset();
    logarithmicExtrema_.reset();
}

//! Set display data (1D)
/*!
 * Set the supplied one-dimensional data (axis points @param x and @param values at those points). The data are copied to local
 * arrays and entities representing the data in the current style are immediately created.
 */
void Data1DEntity::setData(std::vector<double> x, std::vector<double> values, std::optional<std::vector<double>> errors)
{
    clearData();

    // Check vector sizes
    if ((errors.empty()) && (x.size() != values.size()))
        printf("Irregular vector sizes provided (%zu vs %zu) so data will be ignored.\n", x.size(), values.size());
    else if ((x.size() != values.size()) || (x.size() != errors.size()) || (values.size() != errors.size()))
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (errors)) so can't create entities.\n", x.size(),
               values.size(), errors.size());
    {
        x_ = std::move(x);
        values_ = std::move(values);
        errors_ = std::move(errors);
    }

    // Determine data extrema
    auto xit = x_.cbegin(), vit = values_.cbegin();
    while (xit != x_.end())
    {
        extrema_.expand(*xit, *vit, std::nullopt);
        logarithmicExtrema_.expand(*xit <= 0.0 ? std::nullopt : std::optional<double>{log10(*xit)},
                                   *vit <= 0.0 ? std::nullopt : std::optional<double>{log10(*vit)}, std::nullopt);
        ++xit;
        ++vit;
    }

    create();
}

/*
 * Rendering
 */

//! Create renderables in the current style
void Data1DEntity::create()
{
    assert(dataRenderer_);
    dataRenderer_->create(colourDefinition(), x_, xAxis_, values_, valueAxis_);
    assert(errorRenderer_);
    errorRenderer_->create(colourDefinition(), x_, xAxis_, values_, errors_, valueAxis_);
}
