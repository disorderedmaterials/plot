#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <QGridLayout>

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    const auto nRows = 5, nColumns = 5, nPoints = 1000;
    const auto delta = 0.01;

    // Create some data before we add the graphs
    std::vector<double> xValues(nPoints);
    std::iota(xValues.begin(), xValues.end(), 0);
    std::transform(xValues.begin(), xValues.end(), xValues.begin(), [delta](const auto x) { return x * delta; });
    std::vector<std::vector<double>> yValues(nRows * nColumns);
    for (auto &y : yValues)
        y.resize(nPoints);

    // Create a layout for out widgets
    auto *grid = new QGridLayout;
    for (auto row = 0; row < nRows; ++row)
    {
        for (auto column = 0; column < nColumns; ++column)
        {
            auto *graph = new Mildred::MildredWidget;

            // Create some data
            auto a = double(random()) / RAND_MAX;
            auto b = 2.0 * (double(random()) / RAND_MAX);
            auto c = 3.0 * (double(random()) / RAND_MAX);
            auto &y = yValues[nRows * row + column];
            std::transform(xValues.begin(), xValues.end(), y.begin(),
                           [a, b, c](const auto x) { return 5.0 + a * sin(x) + b * sin(x) - c * sin(x); });

            // Add a renderable
            auto *renderable = graph->addData1D("Sines");
            renderable->setData(xValues, y);

            // Add the graph to the layout
            grid->addWidget(graph, row, column);
        }
    }

    // Finally, add the layout to our central widget
    ui_.centralwidget->setLayout(grid);
};
