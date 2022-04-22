#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <QGridLayout>
<<<<<<< HEAD
=======
#include <random>
>>>>>>> df244bb (CI Updates 1 (#24))

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    const auto nRows = 10, nColumns = 10, nPoints = 1000;
    const auto delta = 0.01;

    // Create some data before we add the graphs
    std::vector<double> xValues(nPoints);
    std::iota(xValues.begin(), xValues.end(), 0);
    std::transform(xValues.begin(), xValues.end(), xValues.begin(), [delta](const auto x) { return x * delta; });
    std::vector<std::vector<double>> yValues(nRows * nColumns);
    for (auto &y : yValues)
        y.resize(nPoints);

    // Initialise a random engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    // Create a layout for out widgets
    auto *grid = new QGridLayout;
    for (auto row = 0; row < nRows; ++row)
    {
        for (auto column = 0; column < nColumns; ++column)
        {
            auto *graph = new Mildred::MildredWidget;

            // Create some data
<<<<<<< HEAD
            auto a = double(random()) / RAND_MAX;
            auto b = 2.0 * (double(random()) / RAND_MAX);
            auto c = 3.0 * (double(random()) / RAND_MAX);
=======
            auto a = dist(gen);
            auto b = 2.0 * dist(gen);
            auto c = 3.0 * dist(gen);
>>>>>>> df244bb (CI Updates 1 (#24))
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
