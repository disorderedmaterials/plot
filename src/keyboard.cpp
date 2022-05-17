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
        case (Qt::Key_Up):
        case (Qt::Key_Down):
            if (!flatView_)
                break;
            if (event->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier))
                yAxis_->shiftLimits(yAxis_->range() * (event->key() == Qt::Key_Up ? -0.02 : 0.02));
            else
                yAxis_->shiftLimits(yAxis_->range() * (event->key() == Qt::Key_Up ? -0.1 : 0.1));
            updateTransforms();
            break;
        case (Qt::Key_Left):
        case (Qt::Key_Right):
            if (!flatView_)
                break;
            if (event->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier))
                xAxis_->shiftLimits(xAxis_->range() * (event->key() == Qt::Key_Right ? -0.02 : 0.02));
            else
                xAxis_->shiftLimits(xAxis_->range() * (event->key() == Qt::Key_Right ? -0.1 : 0.1));
            updateTransforms();
            break;
        default:
            break;
    }
}
