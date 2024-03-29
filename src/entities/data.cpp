#include "entities/data.h"

using namespace Mildred;

//! Create a new DataEntity
/*!
 * Construct a new DataEntity storing a reference to the supplied @param metrics and with the given @param parent.
 */
DataEntity::DataEntity(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent)
{
    dataEntity_ = new Qt3DCore::QEntity(this);
    symbolEntity_ = new Qt3DCore::QEntity(this);
    errorEntity_ = new Qt3DCore::QEntity(this);
    positionalTransform_ = new Qt3DCore::QTransform(this);
    addComponent(positionalTransform_);
}

/*
 * Data
 */

//! Return extreme limits for displayed data
Cuboid DataEntity::extrema() const { return extrema_; }

// Return extrema of data points in three-dimensional logarithmic space
Cuboid DataEntity::logarithmicExtrema() const { return logarithmicExtrema_; }

//! Update the limits of the extrema
void DataEntity::updateExtrema(std::optional<double> x, std::optional<double> y, std::optional<double> z)
{
    extrema_.expand(*x, *y, *z);
    logarithmicExtrema_.expand(x <= 0.0 ? std::nullopt : std::optional<double>{log10(*x)},
                               y <= 0.0 ? std::nullopt : std::optional<double>{log10(*y)},
                               z <= 0.0 ? std::nullopt : std::optional<double>{log10(*z)});
}

/*
 * Components
 */

// Return colour definition to use
ColourDefinition DataEntity::colourDefinition() const { return colourOverride_.value_or(colour_); }

// Return local colour definition for entity
ColourDefinition &DataEntity::colour() { return colour_; }
const ColourDefinition &DataEntity::colour() const { return colour_; }

// Set override colour definition (e.g. from group)
void DataEntity::setColourOverride(const ColourDefinition &colour)
{
    colourOverride_ = colour;
    create();
}

// Remove colour definition override
void DataEntity::removeColourOverride()
{
    colourOverride_ = std::nullopt;
    create();
}

// Set data entity material
void DataEntity::setDataMaterial(Qt3DRender::QMaterial *material)
{
    // Remove existing material if one exists
    if (dataEntityMaterial_)
        foreach (auto *node, dataEntity_->childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->removeComponent(dataEntityMaterial_);
        }

    dataEntityMaterial_ = material;
    if (dataEntityMaterial_)
        foreach (auto *node, dataEntity_->childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->addComponent(dataEntityMaterial_);
        }
}

void DataEntity::setErrorMaterial(Qt3DRender::QMaterial *material)
{
    // Remove existing material if one exists
    if (errorEntityMaterial_)
        foreach (auto *node, errorEntity_->childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->removeComponent(errorEntityMaterial_);
        }

    errorEntityMaterial_ = material;
    if (errorEntityMaterial_)
        foreach (auto *node, errorEntity_->childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->addComponent(errorEntityMaterial_);
        }
}

Qt3DRender::QMaterial *DataEntity::dataMaterial() { return dataEntityMaterial_; }

Qt3DRender::QMaterial *DataEntity::errorMaterial() { return errorEntityMaterial_; }

void DataEntity::setSymbolMaterial(Qt3DRender::QMaterial *material)
{
    // Remove existing material if one exists
    if (symbolEntityMaterial_)
        foreach (auto *node, symbolEntity_->childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->removeComponent(symbolEntityMaterial_);
        }

    symbolEntityMaterial_ = material;
    if (symbolEntityMaterial_)
        foreach (auto *node, symbolEntity_->childNodes())
        {
            auto *entity = dynamic_cast<Qt3DCore::QEntity *>(node);
            if (entity)
                entity->addComponent(symbolEntityMaterial_);
        }
}

Qt3DRender::QMaterial *DataEntity::symbolMaterial() { return symbolEntityMaterial_; }

/*
 * Rendering
 */

// Recreate renderables following metric / axis change
void DataEntity::updateRenderables() { create(); }
