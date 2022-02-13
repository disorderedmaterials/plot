#pragma once

#include "renderers/1d/base.h"

//! StyleFactory1D provides access to 1-dimensional data renderers.
/*!
 * StyleFactory1D creates 1D renderer objects on demand which can then be used by a parent entity in order to provide data
 * display.
 */
namespace StyleFactory1D
{
// Styles for 1-dimensional data rendering
enum class Style
{
    Line
};

// Produce renderer for the specified style
std::shared_ptr<DataRenderer1D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity);
}; // namespace StyleFactory1D
