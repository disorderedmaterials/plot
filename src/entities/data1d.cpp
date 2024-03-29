#include "entities/data1d.h"
#include "renderers/1d/stylefactory.h"

using namespace Mildred;

//! Create a new Data1DEntity
/*!
 * Construct a new Data1DEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
Data1DEntity::Data1DEntity(const AxisEntity *xAxis, const AxisEntity *valueAxis, Qt3DCore::QNode *parent,
                           StyleFactory1D::Style style, StyleFactory1D::ErrorBarStyle errorStyle)
    : DataEntity(parent), xAxis_(xAxis), valueAxis_(valueAxis), style_(style), errorStyle_(errorStyle)
{
    dataRenderer_ = StyleFactory1D::createDataRenderer(style_, dataEntity_);
    errorRenderer_ = StyleFactory1D::createErrorRenderer(errorStyle_, errorEntity_);
    symbolRenderer_ = StyleFactory1D::createSymbolRenderer(symbolStyle_, symbolEntity_);
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
    if (x.size() != values.size())
        printf("Irregular vector sizes provided (%zu vs %zu) so data will be ignored.\n", x.size(), values.size());
    else if (errors && x.size() != errors->size())
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (errors)) so can't create entities.\n", x.size(),
               values.size(), errors->size());
    else
    {
        x_ = std::move(x);
        values_ = std::move(values);
        if (errors)
            errors_ = std::move(*errors);
    }

    // Determine data extrema
    auto xit = x_.cbegin(), vit = values_.cbegin(), eit = errors_.cbegin();
    while (xit != x_.end())
    {
        if (errors_.empty())
            updateExtrema(*xit, *vit, std::nullopt);
        else
        {
            updateExtrema(*xit, *vit + *eit, std::nullopt);
            updateExtrema(*xit, *vit - *eit, std::nullopt);
            ++eit;
        }

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
    assert(symbolRenderer_);
    symbolRenderer_->create(colourDefinition(), x_, xAxis_, values_, valueAxis_);
}

//! Set the line style
void Data1DEntity::setLineStyle(StyleFactory1D::Style style)
{
    style_ = style;
    dataRenderer_ = StyleFactory1D::createDataRenderer(style_, dataEntity_);
    if (dataMaterial())
        setDataMaterial(dataMaterial());
    create();
}

//! Set the error style
void Data1DEntity::setErrorStyle(StyleFactory1D::ErrorBarStyle style)
{
    errorStyle_ = style;
    errorRenderer_ = StyleFactory1D::createErrorRenderer(errorStyle_, errorEntity_);
    if (errorMaterial())
        setErrorMaterial(errorMaterial());
    create();
}

//! Set error size
void Data1DEntity::setErrorBarMetric(double metric)
{
    errorRenderer_->setErrorBarMetric(metric);
    create();
}

//! Get error size
double Data1DEntity::errorBarMetric() const { return errorRenderer_->errorBarMetric(); }

//! Set the symbol style
void Data1DEntity::setSymbolStyle(StyleFactory1D::SymbolStyle style)
{
    symbolStyle_ = style;
    symbolRenderer_ = StyleFactory1D::createSymbolRenderer(symbolStyle_, symbolEntity_);
    if (symbolMaterial())
        setSymbolMaterial(symbolMaterial());
    create();
}

//! Set symbol size
void Data1DEntity::setSymbolMetric(double metric)
{
    symbolRenderer_->setSymbolMetric(metric);
    create();
}

//! Get symbol size
double Data1DEntity::symbolMetric() const { return symbolRenderer_->symbolMetric(); }
