#include "mainwindow.h"
#include "material.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include <stdexcept>

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    // View
    connect(ui_.ResetViewButton, SIGNAL(clicked(bool)), ui_.TestingWidget, SLOT(resetView()));
    connect(ui_.ShowSceneCuboidCheck, SIGNAL(clicked(bool)), ui_.TestingWidget, SLOT(setSceneCuboidEnabled(bool)));
    ui_.FlatViewCheck->setChecked(ui_.TestingWidget->isFlatView());
    connect(ui_.FlatViewCheck, SIGNAL(clicked(bool)), ui_.TestingWidget, SLOT(setFlatView(bool)));

    // Axes
    // -- Titles
    ui_.XLabelEdit->setText(ui_.TestingWidget->xAxis()->labelText());
    connect(ui_.XLabelEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setXAxisLabel(const QString &)));
    ui_.YLabelEdit->setText(ui_.TestingWidget->yAxis()->labelText());
    connect(ui_.YLabelEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setYAxisLabel(const QString &)));
    ui_.ZLabelEdit->setText(ui_.TestingWidget->zAxis()->labelText());
    connect(ui_.ZLabelEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setZAxisLabel(const QString &)));
    // -- Visibility
    ui_.XVisibleCheck->setChecked(ui_.TestingWidget->xAxis()->isEnabled());
    connect(ui_.XVisibleCheck, SIGNAL(toggled(bool)), ui_.TestingWidget->xAxis(), SLOT(setEnabled(bool)));
    ui_.YVisibleCheck->setChecked(ui_.TestingWidget->yAxis()->isEnabled());
    connect(ui_.YVisibleCheck, SIGNAL(toggled(bool)), ui_.TestingWidget->yAxis(), SLOT(setEnabled(bool)));
    ui_.ZVisibleCheck->setChecked(ui_.TestingWidget->zAxis()->isEnabled());
    connect(ui_.ZVisibleCheck, SIGNAL(toggled(bool)), ui_.TestingWidget->zAxis(), SLOT(setEnabled(bool)));

    // Mouse Coordinates
    ui_.mouseCoordStyleCombo->addItem(QString("Fixed Anchor"));
    ui_.mouseCoordStyleCombo->addItem(QString("Anchor at Mouse"));
    ui_.mouseCoordStyleCombo->addItem(QString("Use 'External' Slot"));
    ui_.externalMouseCoordLabel->setVisible(false);
    connect(ui_.TestingWidget, SIGNAL(mouseCoordChanged(QPointF)), this, SLOT(setExternalMouseCoordinatesText(QPointF)));

    // Axis Label Rotation
    ui_.XLabelRotationCombo->addItem(QString("Horizontal"));
    ui_.XLabelRotationCombo->addItem(QString("Vertical"));
    ui_.YLabelRotationCombo->addItem(QString("Horizontal"));
    ui_.YLabelRotationCombo->addItem(QString("Vertical"));
    ui_.ZLabelRotationCombo->addItem(QString("Horizontal"));
    ui_.ZLabelRotationCombo->addItem(QString("Vertical"));

    // Data
    const auto nPoints = 1000;
    const auto delta = 2.0 * M_PI / nPoints;
    sinX_.reserve(nPoints);
    sinValues_.reserve(nPoints);
    auto x = 0.0;
    for (auto n = 0; n < nPoints; ++n)
    {
        sinX_.push_back(x);
        sinValues_.push_back(sin(x));
        x += delta;
    }
    auto *sinData = ui_.TestingWidget->addData1D("sin");
    sinData->setData(sinX_, sinValues_);

    ui_.TestingWidget->showAllData();
};

void MainWindow::on_mouseCoordStyleCombo_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui_.TestingWidget->setMouseCoordStyle(Mildred::CoordinateDisplayStyle::FixedAnchor);
            ui_.externalMouseCoordLabel->setVisible(false);
            break;
        case 1:
            ui_.TestingWidget->setMouseCoordStyle(Mildred::CoordinateDisplayStyle::MouseAnchor);
            ui_.externalMouseCoordLabel->setVisible(false);
            break;
        case 2:
            ui_.TestingWidget->setMouseCoordStyle(Mildred::CoordinateDisplayStyle::None);
            ui_.externalMouseCoordLabel->setVisible(true);
            break;
        default:
            throw(std::runtime_error("Unhandled coordinate display style.\n"));
    }
}

void MainWindow::setExternalMouseCoordinatesText(QPointF p)
{
    ui_.externalMouseCoordLabel->setText(QString("%1 %2").arg(QString::number(p.x()), QString::number(p.y())));
}

void MainWindow::on_XLabelRotationCombo_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui_.TestingWidget->xAxis()->setTitleLabelRotation(0.0);
            break;
        case 1:
            ui_.TestingWidget->xAxis()->setTitleLabelRotation(90.0);
            break;
        default:
            throw(std::runtime_error("Unhandled text rotation.\n"));
    }
}

void MainWindow::on_YLabelRotationCombo_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui_.TestingWidget->yAxis()->setTitleLabelRotation(0.0);
            break;
        case 1:
            ui_.TestingWidget->yAxis()->setTitleLabelRotation(90.0);
            break;
        default:
            throw(std::runtime_error("Unhandled text rotation.\n"));
    }
}

void MainWindow::on_ZLabelRotationCombo_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui_.TestingWidget->zAxis()->setTitleLabelRotation(0.0);
            break;
        case 1:
            ui_.TestingWidget->zAxis()->setTitleLabelRotation(90.0);
            break;
        default:
            throw(std::runtime_error("Unhandled text Rotation mode.\n"));
    }
}
