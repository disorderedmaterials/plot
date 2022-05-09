#pragma once

#include "displaygroup.h"
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
    private slots:
    void on_ColourPolicyNoneRadio_clicked(bool checked);
    void on_ColourPolicySingleRadio_clicked(bool checked);
    void on_ColourPolicyVaryingRadio_clicked(bool checked);
    void on_ColourPolicyRGBGradientRadio_clicked(bool checked);
    void on_ColourPolicyHSVGradientRadio_clicked(bool checked);

    private:
    // Main form declaration
    Ui::MainWindow ui_;
    // Display group
    Mildred::DisplayGroup *displayGroup_{nullptr};
};
