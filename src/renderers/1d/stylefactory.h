#pragma once

#include "renderers/1d/base.h"

namespace Mildred
{
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
    None,
    Line
};

enum class ErrorBarStyle
{
    None,
    Stick,
    Tee
};

enum class Position {
    TopLeft=0,
    TopRight=1,
    BottomLeft=2,
    BottomRight=3
};

enum class SymbolStyle
{
    None,
    Circle,
    Square,
    Triangle
};

// Produce renderer for the specified style
std::shared_ptr<DataRenderer1D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity);

// Produce error renderer for the specified style
std::shared_ptr<ErrorRenderer1D> createErrorRenderer(ErrorBarStyle style, Qt3DCore::QEntity *rootEntity);

// Produce symbol renderer for the specified style
std::shared_ptr<SymbolRenderer1D> createSymbolRenderer(SymbolStyle style, Qt3DCore::QEntity *rootEntity);

} // namespace StyleFactory1D
} // namespace Mildred
