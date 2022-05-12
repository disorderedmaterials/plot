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
    if ((x.size() != values.size()) || (x.size() != errors.size()) || (values.size() != errors.size()))
    {
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (errors)) so can't create entities.\n", x.size(), values.size(), errors.size());
        errors_->finalise();
        return {};
    }

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin(), eit = errors.cbegin();
    int i = 0;
    while (xit != x.end())
    {

        // Upper extreme.
        cuboid.expand({float(*xit), float(*vit + *eit), 0.0});
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit + *eit), colour_.colour(*vit));
        errors_->addIndex(i++);

        // Lower extreme.
        cuboid.expand({float(*xit), float(*vit - *eit), 0.0});
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit - *eit), colour_.colour(*vit));
        errors_->addIndex(i++);

        // Add restart index, to cause line break.
        errors_->addIndex(-1);
        ++xit;
        ++vit;
    }

    // Finalise the entity
    errors_->finalise();

    return cuboid;
}
