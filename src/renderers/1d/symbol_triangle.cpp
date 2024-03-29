#include "renderers/1d/symbol_triangle.h"

using namespace Mildred;

TriangleSymbolRenderer1D::TriangleSymbolRenderer1D(Qt3DCore::QEntity *rootEntity) : SymbolRenderer1D(rootEntity)
{
    symbols_ = new LineEntity(rootEntity_);
}

TriangleSymbolRenderer1D::~TriangleSymbolRenderer1D()
{
    if (symbols_)
        symbols_->setParent(static_cast<Qt3DCore::QNode *>(nullptr));
};

/*
 * Rendering
 */

// Create entities from the supplied metrics and data
void TriangleSymbolRenderer1D::create(const ColourDefinition &colour, const std::vector<double> &x, const AxisEntity *xAxis,
                                      const std::vector<double> &values, const AxisEntity *valueAxis)
{
    assert(symbols_);
    symbols_->clear();

    colour_ = colour;

    // Check array sizes
    if (x.size() != values.size())
    {
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y)) so can't create entities.\n", x.size(), values.size());
        symbols_->finalise();
        return;
    }

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin();
    auto i = 0;
    auto w = symbolMetric_ / 2.0;
    while (xit != x.end())
    {
        // Get datapoint value in scaled coordinates
        auto centre = xAxis->toScaled(*xit) + valueAxis->toScaled(*vit);

        // Triangle Vertices
        symbols_->addVertex(centre + QVector3D(-w, 0.755 * w, 0.0), colour_.colour(*vit));
        symbols_->addIndex(i++);
        symbols_->addVertex(centre + QVector3D(w, 0.755 * w, 0.0), colour_.colour(*vit));
        symbols_->addIndex(i++);
        symbols_->addVertex(centre + QVector3D(0.0, -0.755 * w, 0.0), colour_.colour(*vit));
        symbols_->addIndex(i++);

        // Close the triangle!
        symbols_->addIndex(i - 3);

        // Add restart index, to cause line break.
        symbols_->addIndex(-1);
        ++xit;
        ++vit;
    }

    // Finalise the entity
    symbols_->finalise();
}
