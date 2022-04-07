#include "renderers/1d/line.h"

LineRenderer1D::LineRenderer1D(Qt3DCore::QEntity *rootEntity) : DataRenderer1D(rootEntity)
{
    lines_ = new LineEntity(rootEntity_);
}

LineRenderer1D::~LineRenderer1D()
{
    if (lines_)
        lines_->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
};

/*
 * Rendering
 */

// Create entities from the supplied metrics and data
Cuboid LineRenderer1D::create(const std::vector<double> &x, const AxisEntity *xAxis, const std::vector<double> &values,
                              const AxisEntity *valueAxis)
{
    assert(lines_);
    lines_->clear();

    Cuboid cuboid;

    // Check array sizes
    if (x.size() != values.size())
    {
        printf("Irregular vector sizes provided (%zu vs %zu) so can't create entities.\n", x.size(), values.size());
        lines_->finalise();
        return {};
    }

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin();
    while (xit != x.end())
    {
        auto v = xAxis->toScaled(*xit) + valueAxis->toScaled(*vit);
        cuboid.expand(v);
        lines_->addVertex(v);
        ++xit;
        ++vit;
    }

    // Set basic indices
    lines_->setBasicIndices();

    // Finalise the entity
    lines_->finalise();

    return cuboid;
}
