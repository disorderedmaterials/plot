#include "renderers/1d/stylefactory.h"
#include "renderers/1d/error.h"
#include "renderers/1d/line.h"

namespace Mildred
{
namespace StyleFactory1D
{
// Produce renderer for the specified style
std::shared_ptr<DataRenderer1D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity)
{
    if (style == Style::Line)
        return std::make_shared<LineRenderer1D>(rootEntity);
}
std::shared_ptr<ErrorBarRenderer1D> createErrorRenderer(ErrorBarStyle style, Qt3DCore::QEntity *rootEntity)
{
    if (style == ErrorBarStyle::Stick)
        return std::make_shared<ErrorBarRenderer1D>(rootEntity);
}
} // namespace StyleFactory1D
} // namespace Mildred