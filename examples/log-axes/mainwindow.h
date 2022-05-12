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

    private slots:
    void on_XLogarithmicCheck_clicked(bool checked);
    void on_YLogarithmicCheck_clicked(bool checked);
    void on_ZLogarithmicCheck_clicked(bool checked);

    /*
     * Data
     */
    private:
    // Test data
    std::vector<double> modulatedX_, modulatedValues_;
    std::vector<double> steppedX_, steppedValues_;
};
