#include "renderers/1d/stylefactory.h"
#include "renderers/1d/line.h"

namespace StyleFactory1D
{
// Produce renderer for the specified style
std::shared_ptr<DataRenderer1D> createDataRenderer(Style style, Qt3DCore::QEntity *rootEntity)
{
    if (style == Style::Line)
        return std::make_shared<LineRenderer1D>(rootEntity);
}
}; // namespace StyleFactory1D
