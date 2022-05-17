#include "renderers/1d/stylefactory.h"
#include "renderers/1d/error_none.h"
#include "renderers/1d/error_stick.h"
#include "renderers/1d/error_tee.h"
#include "renderers/1d/line.h"
#include "renderers/1d/none.h"

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
}
std::shared_ptr<ErrorRenderer1D> createErrorRenderer(ErrorBarStyle style, Qt3DCore::QEntity *rootEntity)
{
    if (style == ErrorBarStyle::Stick)
        return std::make_shared<StickErrorRenderer1D>(rootEntity);
    else if (style == ErrorBarStyle::Tee)
        return std::make_shared<TeeErrorRenderer1D>(rootEntity);
    else if (style == ErrorBarStyle::None)
        return std::make_shared<NoErrorRenderer1D>(rootEntity);
}
} // namespace StyleFactory1D
} // namespace Mildred