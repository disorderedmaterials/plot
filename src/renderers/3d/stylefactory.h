#pragma once

#include "renderers/3d/base.h"

namespace Mildred
{
//! StyleFactory3D provides access to 3-dimensional data renderers.
/*!
 * StyleFactory3D creates 3D renderer objects on demand which can then be used by a parent entity in order to provide data
 * display.
 */
namespace StyleFactory3D
{
// Styles for 3-dimensional data rendering
enum class Style
{
    None,
    Line
};

// Produce renderer for the specified style
std::shared_ptr<DataRenderer3D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity);

} // namespace StyleFactory3D
} // namespace Mildred
