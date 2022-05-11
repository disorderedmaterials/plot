#include "renderers/1d/error.h"

using namespace Mildred;

ErrorBarRenderer1D(Qt3DCore::QEntity *rootEntity) : ErrorRenderer1DBase(rootEntity) {
    errors_ = new ErrorEntity(rootEntity);
}

ErrorBarRenderer1D::~ErrorBarRenderer1D()
{
    if (errors_)
        errors_->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
};
