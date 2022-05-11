#include "widget.h"
#include <Qt3DInput/QKeyboardHandler>

using namespace Mildred;

void MildredWidget::keyPressEvent(QKeyEvent *event) {}

void MildredWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case (Qt::Key_A):
            // Adjust axis limits to show all data
            showAllData();
            break;
        default:
            break;
    }
}
