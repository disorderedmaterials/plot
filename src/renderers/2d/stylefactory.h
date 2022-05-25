#pragma once

#include "renderers/2d/base.h"

namespace Mildred
{
//! StyleFactory2D provides access to 2-dimensional data renderers.
/*!
 * StyleFactory2D creates 2D renderer objects on demand which can then be used by a parent entity in order to provide data
 * display.
 */
namespace StyleFactory2D
{
// Styles for 2-dimensional data rendering
enum class Style
{
    None,
    Line
};

// Produce renderer for the specified style
std::shared_ptr<DataRenderer2D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity);

} // namespace StyleFactory2D
} // namespace Mildred