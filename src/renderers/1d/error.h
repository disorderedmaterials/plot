#pragma once

#include "entities/line.h"
#include "renderers/1d/base.h"

namespace Mildred
{

class ErrorBarRenderer1D : public ErrorRenderer1DBase
{
    public:
    ErrorBarRenderer1D(Qt3DCore::QEntity *rootEntity);
    ~ErrorBarRenderer1D();
};
} // namespace Mildred