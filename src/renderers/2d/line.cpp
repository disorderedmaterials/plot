#include "renderers/2d/line.h"

using namespace Mildred;

LineRenderer2D::LineRenderer2D(Qt3DCore::QEntity *rootEntity) : DataRenderer2D(rootEntity)
{
    lines_ = new LineEntity(rootEntity_);
}

LineRenderer2D::~LineRenderer2D()
{
    if (lines_)
        lines_->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
};

/*
 * Rendering
 */

// Create entities from the supplied metrics and data
void LineRenderer2D::create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                    const std::vector<double> &y, const AxisEntity *yAxis, const std::vector<double> &values, const AxisEntity *valueAxis)
{
    assert(lines_);
    lines_->clear();

    colour_ = colour;

    // Loop over data and add vertices
    auto xit = x.cbegin(), yit = y.cbegin(), vit = values.cbegin();
    while (xit != x.end())
    {
        lines_->addVertex(xAxis->toScaled(*xit)  + yAxis->toScaled(*yit) + valueAxis->toScaled(*vit), colour_.colour(*vit));
        ++xit;
        ++yit;
        ++vit;
    }

    // Set basic indices
    lines_->setBasicIndices();

    // Finalise the entity
    lines_->finalise();
}