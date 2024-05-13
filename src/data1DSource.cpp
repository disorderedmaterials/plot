#include "data1DSource.h"
#include <cstdio>

using namespace Mildred;

// Set local data
void Data1DSource::setData(std::vector<double> x, std::vector<double> values, std::optional<std::vector<double>> errors)
{
    x_.clear();
    values_.clear();
    errors_.clear();

    // Check vector sizes
    if (x.size() != values.size())
        printf("Irregular vector sizes provided (%zu vs %zu) so data will be ignored.\n", x.size(), values.size());
    else if (errors && x.size() != errors->size())
        printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (errors)) so can't create entities.\n", x.size(),
               values.size(), errors->size());
    else
    {
        x_ = std::move(x);
        values_ = std::move(values);
        if (errors)
            errors_ = std::move(*errors);
    }

    ++version_;
}
