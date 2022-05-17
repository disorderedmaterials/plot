#include "base.h"

using namespace Mildred;

// Returns the error bar metric.
double ErrorRenderer1D::errorBarMetric() const { return errorBarMetric_; }

// Sets the error bar metric.
void ErrorRenderer1D::setErrorBarMetric(double errorBarMetric) { errorBarMetric_ = errorBarMetric; }