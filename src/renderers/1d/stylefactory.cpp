#include "renderers/1d/stylefactory.h"
#include "renderers/1d/line.h"
#include "renderers/1d/error.h"

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
std::shared_ptr<ErrorRenderer1D> createErrorRenderer(ErrorBarStyle style, Qt3DCore::QEntity *rootEntity)
{
    if (style == ErrorBarStyle::ErrorBar)
        return std::make_shared<ErrorRenderer1D>(rootEntity);
}
} // namespace StyleFactory1D
} // namespace Mildred