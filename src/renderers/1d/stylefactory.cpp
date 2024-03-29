#include "renderers/1d/stylefactory.h"
#include "renderers/1d/error_none.h"
#include "renderers/1d/error_stick.h"
#include "renderers/1d/error_tee.h"
#include "renderers/1d/line.h"
#include "renderers/1d/none.h"
#include "renderers/1d/symbol_diamond.h"
#include "renderers/1d/symbol_none.h"
#include "renderers/1d/symbol_square.h"
#include "renderers/1d/symbol_triangle.h"
#include <stdexcept>

namespace Mildred
{
namespace StyleFactory1D
{
// Produce renderer for the specified style
std::shared_ptr<DataRenderer1D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity)
{
    if (style == Style::Line)
        return std::make_shared<LineRenderer1D>(rootEntity);
    else if (style == Style::None)
        return std::make_shared<NoLineRenderer1D>(rootEntity);

    throw(std::runtime_error("DataRenderer1D::createDataRenderer() - Style not accounted for.\n"));
}
std::shared_ptr<ErrorRenderer1D> createErrorRenderer(ErrorBarStyle style, Qt3DCore::QEntity *rootEntity)
{
    if (style == ErrorBarStyle::Stick)
        return std::make_shared<StickErrorRenderer1D>(rootEntity);
    else if (style == ErrorBarStyle::Tee)
        return std::make_shared<TeeErrorRenderer1D>(rootEntity);
    else if (style == ErrorBarStyle::None)
        return std::make_shared<NoErrorRenderer1D>(rootEntity);

    throw(std::runtime_error("DataRenderer1D::createErrorRenderer() - Style not accounted for.\n"));
}
std::shared_ptr<SymbolRenderer1D> createSymbolRenderer(SymbolStyle style, Qt3DCore::QEntity *rootEntity)
{
    if (style == SymbolStyle::None)
        return std::make_shared<NoSymbolRenderer1D>(rootEntity);
    if (style == SymbolStyle::Triangle)
        return std::make_shared<TriangleSymbolRenderer1D>(rootEntity);
    if (style == SymbolStyle::Square)
        return std::make_shared<SquareSymbolRenderer1D>(rootEntity);
    if (style == SymbolStyle::Diamond)
        return std::make_shared<DiamondSymbolRenderer1D>(rootEntity);

    throw(std::runtime_error("DataRenderer1D::createSymbolRenderer() - Style not accounted for.\n"));
}
} // namespace StyleFactory1D
} // namespace Mildred
