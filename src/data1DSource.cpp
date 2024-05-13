#include "data1DSource.h"
#include <cstdio>

using namespace Mildred;

Data1DSource::Data1DSource(const QString &displayName) : displayName_(displayName) {}

/*
 * Basic Information
 */

// Return display name
const QString &Data1DSource::displayName() const { return displayName_; }

/*
 * Local Data
 */

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

    local_ = true;

    if (version_)
        ++(*version_);
    else
        version_ = 0;
}

// Return axis values
const std::vector<double> &Data1DSource::x() const { return x_; }

// Return data values
const std::vector<double> &Data1DSource::values() const { return values_; }

// Return error values
const std::vector<double> &Data1DSource::errors() const { return errors_; }

/*
 * Update
 */

// Update from specified source data if required
bool Data1DSource::updateFrom(const Data1DSource &source)
{
    // For local sources, just copy if necessary
    if (local_)
    {
        printf("UPDATING LOCAL : nX = %i\n", source.x_.size());
        // Check versions
        if (version_.value_or(-1) == source.version_.value_or(-1))
            return false;
        printf("DO WE GET HERE?\n");

        // Check vector sizes
        if (source.x_.size() != source.values_.size())
            printf("Irregular vector sizes provided (%zu vs %zu) so data will be ignored.\n", source.x_.size(),
                   source.values_.size());
        else if (!source.errors_.empty() && (source.x_.size() != source.errors_.size()))
            printf("Irregular vector sizes provided (%zu (x) vs %zu (y) vs %zu (errors)) so can't create entities.\n",
                   source.x_.size(), source.values_.size(), source.errors_.size());

        // Copy data and set version
        x_ = source.x_;
        values_ = source.values_;
        errors_ = source.errors_;
        version_ = source.version_;
    }
    else
    {
        // TODO
    }

    return true;
}
