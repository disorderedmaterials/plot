#pragma once

#include "ui_mainwindow.h"
#include "entities/data1d.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow();
    ~MainWindow() = default;

    /*
     * UI
     */
    private slots:
    void on_UniformErrorRadio_clicked(bool checked);
    void on_RandomErrorRadio_clicked(bool checked);

    private:
    // Main form declaration
    Ui::MainWindow ui_;
    // Display group
    Mildred::Data1DEntity *dataEntity_{nullptr};

    /*
     * Data
     */
    private:
    // Test data
    std::vector<double> xValues_, yValues_, uniformErrors_, randomErrors_;
};
