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
    // dataEntity_->setSymbolStyle(Mildred::StyleFactory1D::SymbolStyle::Triangle);
    // dataEntity_->setSymbolStyle(Mildred::StyleFactory1D::SymbolStyle::Square);
    // dataEntity_->setSymbolStyle(Mildred::StyleFactory1D::SymbolStyle::Diamond);

    ui_.StyleCombo->addItem(QString("Stick"));
    ui_.StyleCombo->addItem(QString("T-Bar Stick"));
    ui_.SymbolCombo->addItem(QString("None"));
    ui_.SymbolCombo->addItem(QString("Triangle"));
    ui_.SymbolCombo->addItem(QString("Square"));
    ui_.SymbolCombo->addItem(QString("Diamond"));
    ui_.WidthSpin->setValue(10.0);
    ui_.SymbolSizeSpin->setValue(12.0);

    connect(ui_.WidthSpin, SIGNAL(valueChanged(double)), this, SLOT(setErrorBarSize(double)));
    connect(ui_.SymbolSizeSpin, SIGNAL(valueChanged(double)), this, SLOT(setSymbolSize(double)));
};

// UI For Errors
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
    dataEntity_->setErrorBarMetric(ui_.WidthSpin->value());
}

void MainWindow::on_StyleCombo_currentIndexChanged(int index)
{
    style_ = index == 0 ? Mildred::StyleFactory1D::ErrorBarStyle::Stick : Mildred::StyleFactory1D::ErrorBarStyle::Tee;
    if (ui_.ShowErrorBarsCheck->isChecked())
    {
        dataEntity_->setErrorStyle(style_);
        dataEntity_->setErrorBarMetric(ui_.WidthSpin->value());
    }
}

void MainWindow::setErrorBarSize(double size) { dataEntity_->setErrorBarMetric(size); }

// UI For Symbols
void MainWindow::on_ShowSymbolsCheck_toggled(bool checked)
{
    dataEntity_->setSymbolStyle(checked ? shapeStyle_ : Mildred::StyleFactory1D::SymbolStyle::None);
    dataEntity_->setSymbolMetric(ui_.SymbolSizeSpin->value());
}

void MainWindow::on_SymbolCombo_currentIndexChanged(int shapeindex)
{
    shapeStyle_ = Mildred::StyleFactory1D::SymbolStyle::None;
    if (shapeindex == 1)
        shapeStyle_ = Mildred::StyleFactory1D::SymbolStyle::Triangle;
    else if (shapeindex == 2)
        shapeStyle_ = Mildred::StyleFactory1D::SymbolStyle::Square;
    else if (shapeindex == 3)
        shapeStyle_ = Mildred::StyleFactory1D::SymbolStyle::Diamond;

    if (ui_.ShowSymbolsCheck->isChecked())
    {
        dataEntity_->setSymbolStyle(shapeStyle_);
        dataEntity_->setSymbolMetric(ui_.SymbolSizeSpin->value());
    }
}

void MainWindow::setSymbolSize(double size) { dataEntity_->setSymbolMetric(size); }
