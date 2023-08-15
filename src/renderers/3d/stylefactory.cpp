#include "renderers/3d/stylefactory.h"
#include "renderers/3d/line.h"

namespace Mildred
{
namespace StyleFactory3D
{
// Produce renderer for the specified style
std::shared_ptr<DataRenderer3D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity)
{
    if (style == Style::Line)
        return std::make_shared<LineRenderer3D>(rootEntity);
}
} // namespace StyleFactory3D
} // namespace Mildred
