#pragma once

#include "entities/data3d.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow();
    ~MainWindow() = default;

    /*
     * UI
     */

    private:
    // Main form declaration
    Ui::MainWindow ui_;

    /*
     * Data
     */
    private:
    // Data limits
    double dataMin_{-5.0}, dataMax_{5.0};
    // Data granularity
    int nPoints_{1000};
    // Test data
    std::vector<double> x_, y_, z_, values_;
    // Entity for data
    Mildred::Data3DEntity *dataEntity_{nullptr};

    public:
    // Function Types
    enum Test3DFunctions
    {
        AckleyFunction
    };
    // Generate specified function
    void generateFunction(Test3DFunctions function);
};
