#include "renderers/1d/line.h"

using namespace Mildred;

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
void LineRenderer1D::create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                            const std::vector<double> &values, const AxisEntity *valueAxis)
{
    assert(lines_);
    lines_->clear();

    colour_ = colour;

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin();
    while (xit != x.end())
    {
        lines_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit), colour_.colour(*vit));
        ++xit;
        ++vit;
    }

    // Set basic indices
    lines_->setBasicIndices();

    // Finalise the entity
    lines_->finalise();
}
