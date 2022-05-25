#pragma once

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
    // Test data
    std::vector<double> sinX_, sinY_, sinValues_;
};
