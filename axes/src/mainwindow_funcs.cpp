#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() : QMainWindow()
{
    ui_.setupUi(this);

    ui_.XTitleEdit->setText(ui_.TestingWidget->xAxis()->titleText());
    connect(ui_.XTitleEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setXAxisTitle(const QString &)));
    ui_.YTitleEdit->setText(ui_.TestingWidget->yAxis()->titleText());
    connect(ui_.YTitleEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setYAxisTitle(const QString &)));
    ui_.ZTitleEdit->setText(ui_.TestingWidget->zAxis()->titleText());
    connect(ui_.ZTitleEdit, SIGNAL(textChanged(const QString &)), ui_.TestingWidget, SLOT(setZAxisTitle(const QString &)));
};
