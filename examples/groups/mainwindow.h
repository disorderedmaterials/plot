#pragma once

#include "displaygroup.h"
#include "entities/data1d.h"
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
    void on_ShowSin1Check_toggled(bool checked);
    void on_ShowSin2Check_toggled(bool checked);
    void on_ShowSin3Check_toggled(bool checked);
    void on_ShowSin4Check_toggled(bool checked);
    void on_ShowSin5Check_toggled(bool checked);
    void on_ShowSin6Check_toggled(bool checked);

    private:
    // Main form declaration
    Ui::MainWindow ui_;
    // Display group
    Mildred::DisplayGroup *displayGroup_{nullptr};

    // Data entities
    std::vector<Mildred::Data1DEntity*> entities_;

};
