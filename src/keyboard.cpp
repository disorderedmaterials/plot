#include "widget.h"
#include <Qt3DInput/QKeyboardHandler>

using namespace Mildred;

void MildredWidget::keyPressEvent(QKeyEvent *event) {}

void MildredWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case (Qt::Key_A):
            break;
        default:
            break;
    }
}
