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
    private slots:
    void on_mouseCoordStyleCombo_currentIndexChanged(int index);
    void on_XLabelAlignmentCombo_currentIndexChanged(int index);
    void on_YLabelAlignmentCombo_currentIndexChanged(int index);
    void on_ZLabelAlignmentCombo_currentIndexChanged(int index);

    public slots:
    // Set mouse coordinates in external label.
    void setExternalMouseCoordinatesText(QPointF p);

    private:
    // Main form declaration
    Ui::MainWindow ui_;

    /*
     * Data
     */
    private:
    // Test data
    std::vector<double> sinX_, sinValues_;
};
