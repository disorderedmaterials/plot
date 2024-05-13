#include "data.h"
#include "data1DSource.h"

using namespace Mildred;

/*
 * Source 1D Data
 */

// Create new data 1D source
Mildred::Data1DSource *DataSource::addData1D()
{
    return data1D_.emplace_back(std::make_unique<Data1DSource>()).get();
}

/*
 * Grouping
 */

// Create new display group
DisplayGroup *DataSource::addDisplayGroup()
{
    auto newGroup = displayGroups_.emplace_back(std::make_shared<DisplayGroup>());

    return newGroup.get();
}
