#include "base.h"

using namespace Mildred;

// Returns the symbol metric.
double SymbolRenderer1D::symbolMetric() const { return symbolMetric_; }

// Sets the symbol metric.
void SymbolRenderer1D::setSymbolMetric(double symbolMetric) { symbolMetric_ = symbolMetric; }
