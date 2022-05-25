#include "mainwindow.h"
#include "material.h"
#include "ui_mainwindow.h"
#include "entities/axis.h"
#include "widget.h"
#include <stdexcept>

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    // Data
    const auto nPoints = 1000;
    const auto delta = 2.0 * M_PI / nPoints;
    sinX_.reserve(nPoints);
    sinY_.reserve(nPoints);
    sinValues_.reserve(nPoints);
    auto x = 0.0;
    for (auto n = 0; n < nPoints; ++n)
    {
        sinX_.push_back(x);
        sinY_.push_back(x);
        sinValues_.push_back(sin(n));
        x += delta;
    }
    auto *sinData = ui_.TestingWidget->addData2D("sin");
    sinData->setData(sinX_, sinY_, sinValues_);

    // ui_.TestingWidget->zAxis()->setEnabled(true);
    // ui_.TestingWidget->zAxis()->setType(Mildred::AxisEntity::AxisType::Vertical);
    ui_.TestingWidget->yAxis()->setType(Mildred::AxisEntity::AxisType::Depth);

    ui_.TestingWidget->showAllData();
};  