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
                                const std::vector<double> &values, const std::vector<double> &symbols,
                                const AxisEntity *valueAxis)
{
    assert(symbols_);
    symbols_->clear();

    colour_ = colour;

    // Check if symbols are defined.
    if (symbols.empty())
    {
        symbols_->finalise();
        return;
    }

    // Check array sizes
    if ((x.size() != values.size()) || (x.size() != symbols.size()) || (values.size() != symbols.size()))
    {
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (symbols)) so can't create entities.\n", x.size(),
               values.size(), symbols.size());
        symbols_->finalise();
        return;
    }

    // Loop over data and add vertices
    auto xit = x.cbegin(), vit = values.cbegin(), eit = symbols.cbegin();
    auto i = 0;
    double w = Size/2.0;
    while (xit != x.end())
    {
        //Triangle Vertices
        symbols_->addVertex(xAxis-w, valueAxis+0.755*w, xAxis+w, valueAxis+0.755*w);
        symbols_->addVertex(xAxis+w, valueAxis+0.755*w,   xAxis, valueAxis-0.977*w);
        symbols_->addVertex(  xAxis, valueAxis-0.977*w, xAxis-w, valueAxis+0.755*w);
        symbols_->addIndex(i++);
	      symbols_->addIndex(i++);
	      symbols_->addIndex(i++);

        // Add restart index, to cause line break.
        symbols_->addIndex(-1);
        ++xit;
        ++vit;
        ++eit;
    }

    // Finalise the entity
    symbols_->finalise();
}
