#include "renderers/1d/error.h"

using namespace Mildred;

ErrorBarRenderer1D::ErrorBarRenderer1D(Qt3DCore::QEntity *rootEntity) : ErrorRenderer1D(rootEntity)
{
    errors_ = new LineEntity(rootEntity_);
}

ErrorBarRenderer1D::~ErrorBarRenderer1D()
{
    if (errors_)
        errors_->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
};

/*
 * Rendering
 */

// Create entities from the supplied metrics and data
Cuboid ErrorBarRenderer1D::create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                              const std::vector<double> &values, const std::vector<double> &errors, const AxisEntity *valueAxis)
{
    assert(errors_);
    errors_->clear();

    Cuboid cuboid;
    colour_ = colour;

    // Check array sizes
    if (x.size() != values.size())
    {
        printf("Irregular vector sizes provided (%zu vs %zu) so can't create entities.\n", x.size(), values.size());
        errors_->finalise();
        return {};
    }

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin(), eit = errors.cbegin();
    int i = 0;
    while (xit != x.end())
    {

        cuboid.expand({float(*xit), float(*vit + *eit), 0.0});
        cuboid.expand({float(*xit), float(*vit - *eit), 0.0});
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit + *eit), colour_.colour(*vit));
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit - *eit), colour_.colour(*vit));
        errors_->addIndex(i);
        errors_->addIndex(i+1);
        errors_->addIndex();
        ++i;
        ++i;
        ++xit;
        ++vit;
    }

    // Set basic indices
    // errors_->setBasicIndices();

    // Finalise the entity
    errors_->finalise();

    return cuboid;
}
