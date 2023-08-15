#include "renderers/2d/stylefactory.h"
#include "renderers/2d/line.h"

namespace Mildred
{
namespace StyleFactory2D
{
// Produce renderer for the specified style
std::shared_ptr<DataRenderer2D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity)
{
    if (style == Style::Line)
        return std::make_shared<LineRenderer2D>(rootEntity);
}
} // namespace StyleFactory2D
} // namespace Mildred
