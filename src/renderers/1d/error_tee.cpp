#include "renderers/1d/error_tee.h"

using namespace Mildred;

TeeErrorRenderer1D::TeeErrorRenderer1D(Qt3DCore::QEntity *rootEntity) : ErrorRenderer1D(rootEntity)
{
    errors_ = new LineEntity(rootEntity_);
}

TeeErrorRenderer1D::~TeeErrorRenderer1D()
{
    if (errors_)
        errors_->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
};

/*
 * Rendering
 */

// Create entities from the supplied metrics and data
void TeeErrorRenderer1D::create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                                const std::vector<double> &values, const std::vector<double> &errors,
                                const AxisEntity *valueAxis)
{
    assert(errors_);
    errors_->clear();

    colour_ = colour;

    // Check if errors are defined.
    if (errors.empty())
    {
        errors_->finalise();
        return;
    }

    // Check array sizes
    if ((x.size() != values.size()) || (x.size() != errors.size()) || (values.size() != errors.size()))
    {
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (errors)) so can't create entities.\n", x.size(),
               values.size(), errors.size());
        errors_->finalise();
        return;
    }

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin(), eit = errors.cbegin();
    auto i = 0;
    while (xit != x.end())
    {
        // Upper extreme.
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit + *eit), colour_.colour(*vit));
        errors_->addIndex(i++);

        // Lower extreme.
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit - *eit), colour_.colour(*vit));
        errors_->addIndex(i++);

        // Add restart index, to cause line break.
        errors_->addIndex(-1);

        // Upper T bar.
        // Offset x of vertex by +errorBarMetric_ / 2.
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit + *eit) + QVector3D(errorBarMetric() / 2.0, 0, 0),
                           colour_.colour(*vit));
        errors_->addIndex(i++);
        // Offset x of vertex by -errorBarMetric_ / 2.
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit + *eit) + QVector3D(-errorBarMetric() / 2.0, 0, 0),
                           colour_.colour(*vit));
        errors_->addIndex(i++);
        // Add restart index, to cause line break.
        errors_->addIndex(-1);

        // Lower T bar.
        // Offset x of vertex by +errorBarMetric_ / 2.
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit - *eit) + QVector3D(errorBarMetric() / 2.0, 0, 0),
                           colour_.colour(*vit));
        errors_->addIndex(i++);
        // Offset x of vertex by -errorBarMetric_ / 2.
        errors_->addVertex(xAxis->toScaled(*xit) + valueAxis->toScaled(*vit - *eit) + QVector3D(-errorBarMetric() / 2.0, 0, 0),
                           colour_.colour(*vit));
        errors_->addIndex(i++);
        // Add restart index, to cause line break.
        errors_->addIndex(-1);

        ++xit;
        ++vit;
        ++eit;
    }

    // Finalise the entity
    errors_->finalise();
}