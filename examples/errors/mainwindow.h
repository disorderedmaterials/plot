#pragma once

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
    void on_UniformErrorRadio_clicked(bool checked);
    void on_RandomErrorRadio_clicked(bool checked);
    void on_ShowErrorBarsCheck_toggled(bool checked);
    void on_ShowSymbolsCheck_selected(bool checked);
    void on_StyleCombo_currentIndexChanged(int index);
    void on_SymbolStyleCombo_currentShapeIndexChanged(int shapeindex);
    

    public slots:
    // Set error size
    void setErrorBarSize(double size);

    // Set symbol size
    void setSymbolSize(double size);

    private:
    // Main form declaration
    Ui::MainWindow ui_;
    // Display group
    Mildred::Data1DEntity *dataEntity_{nullptr};
    // Style
    Mildred::StyleFactory1D::ErrorBarStyle style_{Mildred::StyleFactory1D::ErrorBarStyle::Stick};
    Mildred::StyleFactory1D::SymbolStyle shapestyle_{Mildred::StyleFactory1D::SymbolStyle::Triangle};
    /*
     * Data
     */
    private:
    // Test data
    std::vector<double> xValues_, yValues_, uniformErrors_, randomErrors_;
};
