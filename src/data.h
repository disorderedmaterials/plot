#pragma once

#include "data1DSource.h"
#include "displaygroup.h"
#include <QObject>

namespace Mildred
{
//! DataSource is the main interface class for passing data to MildredWidget.
/*!
 * DataSource provides access to all data to be displayed on a given MildredWidget.
 * TODO
 */
class DataSource : public QObject
{
    Q_OBJECT

    public:
    DataSource() = default;
    ~DataSource() = default;

    /*
     * Source 1D Data
     */
    private:
    std::vector<std::unique_ptr<Data1DSource>> data1D_;

    public:
    // Create new data 1D source
    Data1DSource *addData1D();

    /*
     * Grouping
     */
    private:
    // Defined display groups
    std::vector<std::shared_ptr<DisplayGroup>> displayGroups_;

    public:
    // Create new display group
    DisplayGroup *addDisplayGroup();
};
} // namespace Mildred
