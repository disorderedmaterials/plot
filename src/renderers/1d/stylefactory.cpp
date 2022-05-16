#include "renderers/1d/stylefactory.h"
#include "renderers/1d/error.h"
#include "renderers/1d/error_t_stick.h"
#include "renderers/1d/line.h"
#include "renderers/1d/noerror.h"
#include "renderers/1d/noline.h"

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
        return std::make_shared<ErrorBarRenderer1D>(rootEntity);
    else if (style == ErrorBarStyle::T_Stick)
        return std::make_shared<ErrorTStickRenderer1D>(rootEntity);
    else if (style == ErrorBarStyle::None)
        return std::make_shared<NoErrorRenderer1D>(rootEntity);
}
} // namespace StyleFactory1D
} // namespace Mildred