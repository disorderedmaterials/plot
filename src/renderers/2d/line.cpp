#include "renderers/2d/line.h"

using namespace Mildred;

LineRenderer2D::LineRenderer2D(Qt3DCore::QEntity *rootEntity) : DataRenderer2D(rootEntity)
{
    lines_ = new LineEntity(rootEntity_);
}

LineRenderer2D::~LineRenderer2D()
{
    // for (auto &line : lines_)
    //     line->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
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
    // for (auto &line : lines_)
    // {
    //     assert(line);
    //     line->clear();
    // }
    // lines_->clear();
    // return;
    colour_ = colour;

    // Loop over data and add vertices
    auto xit = x.cbegin(), yit = y.cbegin(), vit = values.cbegin();
    // printf("%d %d\n", x.size(), lines_.size());

    // int x_ = x.size();
    // lines_.resize(1);
    // LineEntity* line = new LineEntity(rootEntity_);
    // line->addVertex({50, 50, 0}, colour_.colour(50));
    // line->setBasicIndices();
    // line->finalise();
    // lines_.emplace_back(line);
    // lines_.resize(x_);
    while (xit != x.end())
    {
        lines_->addVertex(xAxis->toScaled(*xit) + yAxis->toScaled(*yit) + valueAxis->toScaled(*vit), colour_.colour(*vit));
        // LineEntity* line = new LineEntity(rootEntity_);
        // line->addVertex(xAxis->toScaled(*xit)  + yAxis->toScaled(*yit), colour_.colour(*vit));
        ++xit;
        ++yit;
        ++vit;
        // line->setBasicIndices();
        // line->finalise();
        // lines_.emplace_back(line);
    }

    // printf("%d\n", lines_.size());
    // Set basic indices
    lines_->setBasicIndices();

    // Finalise the entity
    lines_->finalise();
}