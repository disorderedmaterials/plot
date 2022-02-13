#include "entities/data.h"

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
std::optional<Cuboid> DataEntity::extrema() const { return extrema_; }

/*
 * Rendering
 */

// Recreate renderables following metric / axis change
void DataEntity::updateRenderables() { create(); }
