#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    // View
    connect(ui_.ResetViewButton, SIGNAL(clicked(bool)), ui_.TestingWidget, SLOT(resetView()));
    connect(ui_.ShowSceneCuboidCheck, SIGNAL(clicked(bool)), ui_.TestingWidget, SLOT(setSceneCuboidEnabled(bool)));
    ui_.FlatViewCheck->setChecked(ui_.TestingWidget->isFlatView());
    connect(ui_.FlatViewCheck, SIGNAL(clicked(bool)), ui_.TestingWidget, SLOT(setFlatView(bool)));

    // Axes
    ui_.XTitleEdit->setText(ui_.TestingWidget->xAxis()->titleText());
    connect(ui_.XTitleEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setXAxisTitle(const QString &)));
    ui_.YTitleEdit->setText(ui_.TestingWidget->yAxis()->titleText());
    connect(ui_.YTitleEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setYAxisTitle(const QString &)));
    ui_.ZTitleEdit->setText(ui_.TestingWidget->zAxis()->titleText());
    connect(ui_.ZTitleEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setZAxisTitle(const QString &)));
    ui_.XVisibleCheck->setChecked(ui_.TestingWidget->xAxis()->isEnabled());
    connect(ui_.XVisibleCheck, SIGNAL(toggled(bool)), ui_.TestingWidget->xAxis(), SLOT(setEnabled(bool)));
    ui_.YVisibleCheck->setChecked(ui_.TestingWidget->yAxis()->isEnabled());
    connect(ui_.YVisibleCheck, SIGNAL(toggled(bool)), ui_.TestingWidget->yAxis(), SLOT(setEnabled(bool)));
    ui_.ZVisibleCheck->setChecked(ui_.TestingWidget->zAxis()->isEnabled());
    connect(ui_.ZVisibleCheck, SIGNAL(toggled(bool)), ui_.TestingWidget->zAxis(), SLOT(setEnabled(bool)));

    // Data
    for (auto n = -5; n <= 5; ++n)
    {
        squaresX_.push_back(n);
        squaresValues_.push_back(fabs(n));
    }
    auto *squares = ui_.TestingWidget->addData1D("Squares");
    squares->setData(squaresX_, squaresValues_);
    auto *c = new Qt3DExtras::QDiffuseSpecularMaterial(squares);
    c->setAmbient(Qt::blue);
    squares->addComponent(c);
};
