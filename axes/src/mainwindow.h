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

    protected:
    //void closeEvent(QCloseEvent *event);
    //void resizeEvent(QResizeEvent *event);
};
