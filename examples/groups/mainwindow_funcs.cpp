#include "mainwindow.h"
#include "material.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <random>

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    const auto nDataSets = 6, nPoints = 1000;
    const auto delta = 0.01;

    // Set up our data containers
    std::vector<double> xValues(nPoints);
    std::iota(xValues.begin(), xValues.end(), 0);
    std::transform(xValues.begin(), xValues.end(), xValues.begin(), [delta](const auto x) { return x * delta; });
    std::vector<std::vector<double>> yValues(nDataSets);
    for (auto &y : yValues)
        y.resize(nPoints);

    // Initialise a random engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    // Create a display group with some default policies
    displayGroup_ = ui_.TestingWidget->addDisplayGroup();
    displayGroup_->setSingleColour({255, 0, 200, 255});

    // Create the datasets
    for (auto n = 0; n < nDataSets; ++n)
    {
        // Create some data
        auto a = dist(gen);
        auto b = 2.0 * dist(gen);
        auto c = 3.0 * dist(gen);
        auto &y = yValues[n];
        std::transform(xValues.begin(), xValues.end(), y.begin(),
                       [a, b, c](const auto x) { return 5.0 + a * sin(x) + b * sin(2.0 * x) - c * sin(3.0 * x); });

        // Create a renderable and add it to the group
        auto *renderable = ui_.TestingWidget->addData1D(std::string("Sines") + std::to_string(n));
        renderable->setData(xValues, y);

        renderable->colour().set({int(255 * dist(gen)), int(255 * dist(gen)), int(255 * dist(gen)), 255});
        displayGroup_->addTarget(renderable);

        QCheckBox *checkBox = new QCheckBox(QString((std::string("Sines") + std::to_string(n)).c_str()));
        checkBox->setChecked(true);
        ui_.datasetsLayout->addWidget(checkBox);
        connect(checkBox, &QCheckBox::stateChanged, [=](int state) {
            renderable->setLineStyle(state == Qt::Checked ? Mildred::StyleFactory1D::Style::Line
                                                          : Mildred::StyleFactory1D::Style::None);
        });
        // connect(checkBox, SIGNAL(clicked(bool)), renderable->setErrorStyle(Mildred::StyleFactory1D::Style::None))
    }
};

void MainWindow::on_ColourPolicyNoneRadio_clicked(bool checked)
{
    if (checked)
        displayGroup_->setColourPolicy(Mildred::DisplayGroup::ColourPolicy::None);
}

void MainWindow::on_ColourPolicySingleRadio_clicked(bool checked)
{
    if (checked)
        displayGroup_->setColourPolicy(Mildred::DisplayGroup::ColourPolicy::Single);
}

void MainWindow::on_ColourPolicyVaryingRadio_clicked(bool checked)
{
    if (checked)
        displayGroup_->setColourPolicy(Mildred::DisplayGroup::ColourPolicy::Varying);
}

void MainWindow::on_ColourPolicyRGBGradientRadio_clicked(bool checked)
{
    if (checked)
    {
        Mildred::ColourDefinition gradient;
        gradient.addPoint(0.0, Qt::darkRed);
        gradient.addPoint(2.5, Qt::darkGreen);
        gradient.addPoint(5.0, Qt::darkYellow);
        gradient.addPoint(7.5, Qt::darkMagenta);
        gradient.addPoint(10.0, Qt::darkBlue);
        displayGroup_->setGradient(gradient);
        displayGroup_->setColourPolicy(Mildred::DisplayGroup::ColourPolicy::Gradient);
    }
}

void MainWindow::on_ColourPolicyHSVGradientRadio_clicked(bool checked)
{
    if (checked)
    {
        Mildred::ColourDefinition gradient;
        gradient.setUseHSV(true);
        gradient.addPoint(0.0, Qt::darkRed);
        gradient.addPoint(10.0, Qt::darkBlue);
        displayGroup_->setGradient(gradient);
        displayGroup_->setColourPolicy(Mildred::DisplayGroup::ColourPolicy::Gradient);
    }
}