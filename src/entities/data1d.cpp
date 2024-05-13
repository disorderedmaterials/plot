#include "entities/data1d.h"
#include "renderers/1d/stylefactory.h"

using namespace Mildred;

//! Create a new Data1DEntity
/*!
 * Construct a new Data1DEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
Data1DEntity::Data1DEntity(const Data1DSource *source, const AxisEntity *xAxis, const AxisEntity *valueAxis,
                           Qt3DCore::QNode *parent, StyleFactory1D::Style style, StyleFactory1D::ErrorBarStyle errorStyle)
    : DataEntity(parent), sourceData_(source), xAxis_(xAxis), valueAxis_(valueAxis), style_(style), errorStyle_(errorStyle)
{
    dataRenderer_ = StyleFactory1D::createDataRenderer(style_, dataEntity_);
    errorRenderer_ = StyleFactory1D::createErrorRenderer(errorStyle_, errorEntity_);
    symbolRenderer_ = StyleFactory1D::createSymbolRenderer(symbolStyle_, symbolEntity_);

    updateFromSourceData();
}

/*
 * Data
 */

//! Update from source data
/*!
 * Update our local data arrays from the specified source. Entities representing the data in the current style are immediately
 * created.
 */
bool Data1DEntity::updateFromSourceData()
{
    assert(sourceData_);

    // Update our local data from the source
    if (!data_.updateFrom(*sourceData_))
        return false;

    // Clear and recalculate extrema
    extrema_.reset();
    logarithmicExtrema_.reset();

    auto xit = data_.x().cbegin(), vit = data_.values().cbegin(), eit = data_.errors().cbegin();
    auto hasErrors = !data_.errors().empty();
    while (xit != data_.x().end())
    {
        if (hasErrors)
        {
            updateExtrema(*xit, *vit + *eit, std::nullopt);
            updateExtrema(*xit, *vit - *eit, std::nullopt);
            ++eit;
        }
        else
            updateExtrema(*xit, *vit, std::nullopt);

        ++xit;
        ++vit;
    }

    create();

    return true;
}

/*
 * Rendering
 */

//! Create renderables in the current style
void Data1DEntity::create()
{
    assert(dataRenderer_);
    dataRenderer_->create(colourDefinition(), data_.x(), xAxis_, data_.values(), valueAxis_);
    assert(errorRenderer_);
    errorRenderer_->create(colourDefinition(), data_.x(), xAxis_, data_.values(), data_.errors(), valueAxis_);
    assert(symbolRenderer_);
    symbolRenderer_->create(colourDefinition(), data_.x(), xAxis_, data_.values(), valueAxis_);
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
