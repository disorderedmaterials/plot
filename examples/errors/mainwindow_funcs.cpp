#include "mainwindow.h"
#include "material.h"
#include "renderers/1d/stylefactory.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <random>

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    // Initialise a random engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    // Data
    const auto nPoints = 10;
    double relativeError = 0.1;

    // Fill in x values (0 to nPoints).
    xValues_ = std::vector<double>(nPoints);
    std::iota(xValues_.begin(), xValues_.end(), 0);

    // Fill in y values (sin(x)).
    yValues_ = std::vector<double>(nPoints);
    std::transform(xValues_.begin(), xValues_.end(), yValues_.begin(), [](const auto x) { return sin(x); });

    // Fill in uniform errors.
    uniformErrors_ = std::vector<double>(nPoints);
    std::transform(yValues_.begin(), yValues_.end(), uniformErrors_.begin(),
                   [relativeError](const auto x) { return relativeError * x; });

    // Fill in random errors.
    randomErrors_ = std::vector<double>(nPoints);
    std::transform(yValues_.begin(), yValues_.end(), randomErrors_.begin(),
                   [dist, gen](const auto x) mutable { return x * dist(gen); });

    dataEntity_ = ui_.TestingWidget->addData1D("Sin");
    dataEntity_->setData(xValues_, yValues_, uniformErrors_);
};

void MainWindow::on_UniformErrorRadio_clicked(bool checked)
{
    if (checked)
        dataEntity_->setData(xValues_, yValues_, uniformErrors_);
}

void MainWindow::on_RandomErrorRadio_clicked(bool checked)
{
    if (checked)
        dataEntity_->setData(xValues_, yValues_, randomErrors_);
}

void MainWindow::on_ShowErrorBarsCheck_toggled(bool checked)
{
    if (checked)
        dataEntity_->setErrorStyle(Mildred::StyleFactory1D::ErrorBarStyle::Stick);
    else
        dataEntity_->setErrorStyle(Mildred::StyleFactory1D::ErrorBarStyle::None);
}