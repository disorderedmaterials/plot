#include "mainwindow.h"
#include "material.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    // Axes
    ui_.XLogarithmicCheck->setChecked(ui_.TestingWidget->xAxis()->isLogarithmic());
    ui_.YLogarithmicCheck->setChecked(ui_.TestingWidget->yAxis()->isLogarithmic());
    ui_.ZLogarithmicCheck->setChecked(ui_.TestingWidget->zAxis()->isLogarithmic());

    // Data
    const auto delta = 0.1;
    const int nPoints = 10000;
    modulatedX_.reserve(nPoints);
    modulatedValues_.reserve(nPoints);
    auto x = 0.0;
    for (auto n = 0; n < nPoints; ++n)
    {
        modulatedX_.push_back(x);
        modulatedValues_.push_back(sin(x) * sin(x / 10.0));
        steppedX_.push_back(x);
        steppedValues_.push_back(x);
        steppedX_.push_back(x + delta);
        steppedValues_.push_back(x);
        x += delta;
    }
    auto *modSin = ui_.TestingWidget->addData1D("Modulated");
    modSin->setData(modulatedX_, modulatedValues_);
    auto *stepped = ui_.TestingWidget->addData1D("Stepped");
    stepped->setData(steppedX_, steppedValues_);
    stepped->setEnabled(false);

    // Data Checkboxes
    ui_.ModulatedSineCheck->setChecked(modSin->isEnabled());
    connect(ui_.ModulatedSineCheck, SIGNAL(clicked(bool)), modSin, SLOT(setEnabled(bool)));
    ui_.StepsCheck->setChecked(stepped->isEnabled());
    connect(ui_.StepsCheck, SIGNAL(clicked(bool)), stepped, SLOT(setEnabled(bool)));

    ui_.TestingWidget->showAllData();
};

void MainWindow::on_XLogarithmicCheck_clicked(bool checked) { ui_.TestingWidget->xAxis()->setLogarithmic(checked); }

void MainWindow::on_YLogarithmicCheck_clicked(bool checked) { ui_.TestingWidget->yAxis()->setLogarithmic(checked); }

void MainWindow::on_ZLogarithmicCheck_clicked(bool checked) { ui_.TestingWidget->zAxis()->setLogarithmic(checked); }