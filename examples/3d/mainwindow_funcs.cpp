#include "mainwindow.h"
#include "material.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    generateFunction(Test3DFunctions::AckleyFunction);
    dataEntity_ = ui_.TestingWidget->addData3D("3D Function");
    dataEntity_->setData(x_, y_, z_, values_);

    ui_.TestingWidget->zAxis()->setEnabled(true);
    ui_.TestingWidget->zAxis()->setType(Mildred::AxisEntity::AxisType::Vertical);
    //ui_.TestingWidget->yAxis()->setType(Mildred::AxisEntity::AxisType::Depth);

    ui_.TestingWidget->showAllData();
};

// Generate specified function
void MainWindow::generateFunction(Test3DFunctions function)
{
    // Initialise the data and calculate the delta
    // Data, all bounded to [-5,5] in x and y
    const auto axisMin = -50.0, axisMax = 50.0, delta = (axisMax - axisMin) / (nPoints_ - 1);

    x_.reserve(nPoints_);
    x_.clear();
    values_.reserve(nPoints_ * nPoints_);
    values_.clear();

    // Generate x axis and duplicate as y
    for (auto n = 0; n <= nPoints_; ++n)
    x_.push_back(n * delta);
    y_ = x_;

    switch (function)
    {
        case (Test3DFunctions::AckleyFunction):
            /*
             * Ackley's Function
             * f(x,y) = -20.0 * exp(-0.2 * sqrt(0.5 * (x^2 + y^2))) - exp(0.5 * (cos(2 * M_PI * x) + cos(2 * M_PI * y))) + e +
             * 20
             */
            for (const auto x : x_)
                for (const auto y : y_)
                    values_.push_back(-20.0 * exp(-0.2 * sqrt(0.5 * (x * x + y * y))) -
                                      exp(0.5 * (cos(2 * M_PI * x) + cos(2 * M_PI * y))) + M_E + 20);
            break;
    }
}
