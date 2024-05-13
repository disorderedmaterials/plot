#include "data.h"
#include "data1DSource.h"

using namespace Mildred;

/*
 * Source 1D Data
 */

// Create new data 1D source
Mildred::Data1DSource *DataSource::addData1DSource()
{
    return data1DSources_.emplace_back(std::make_unique<Data1DSource>()).get();
}

// Return 1D data sources
const std::vector<std::unique_ptr<Data1DSource>> &DataSource::data1DSources() const { return data1DSources_; }

/*
 * Grouping
 */

// Create new display group
DisplayGroup *DataSource::addDisplayGroup()
{
    auto newGroup = displayGroups_.emplace_back(std::make_shared<DisplayGroup>());

    return newGroup.get();
}
