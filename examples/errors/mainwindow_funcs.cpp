#include "mainwindow.h"
#include "material.h"
#include "renderers/1d/stylefactory.h"
#include "ui_mainwindow.h"
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
    xValues_.resize(nPoints);
    std::iota(xValues_.begin(), xValues_.end(), 0);

    // Fill in y values (sin(x)).
    yValues_.resize(nPoints);
    std::transform(xValues_.begin(), xValues_.end(), yValues_.begin(), [](const auto x) { return sin(x); });

    // Fill in uniform errors.
    uniformErrors_.resize(nPoints);
    std::transform(yValues_.begin(), yValues_.end(), uniformErrors_.begin(),
                   [relativeError](const auto x) { return relativeError * x; });

    // Fill in random errors.
    randomErrors_.resize(nPoints);
    std::transform(yValues_.begin(), yValues_.end(), randomErrors_.begin(),
                   [dist, gen](const auto x) mutable { return x * dist(gen); });

    dataEntity_ = ui_.TestingWidget->addData1D("Sin");
    dataEntity_->setData(xValues_, yValues_, uniformErrors_);

    ui_.StyleCombo->addItem(QString("Stick"));
    ui_.StyleCombo->addItem(QString("T-Bar Stick"));

    ui_.WidthSpin->setValue(10.0);
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
    dataEntity_->setErrorStyle(checked ? style_ : Mildred::StyleFactory1D::ErrorBarStyle::None);
    dataEntity_->setErrorBarSize(float(ui_.WidthSpin->value()));

}

void MainWindow::on_StyleCombo_currentIndexChanged(int index)
{
    style_ = index == 0 ? Mildred::StyleFactory1D::ErrorBarStyle::Stick : Mildred::StyleFactory1D::ErrorBarStyle::T_Stick;
    if (ui_.ShowErrorBarsCheck->isChecked()) {
        dataEntity_->setErrorStyle(style_);
        dataEntity_->setErrorBarSize(float(ui_.WidthSpin->value()));
    }
}

void MainWindow::on_WidthSpin_valueChanged(double value)
{
    dataEntity_->setErrorBarSize(float(value));
}