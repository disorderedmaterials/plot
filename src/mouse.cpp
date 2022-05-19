#include "widget.h"

using namespace Mildred;

//! React to mouse movement
/*!
 * React to mouse move events signalled by a Qt3DMouseHandler attached to the Qt3D surface, potentially affecting changes to the
 * axes ranges or view volume, depending on the current view type and options.
 *
 * Active mouse buttons modify the effect of the event, and which also depends on the current view mode and depressed modifier
 * keys.
 *
 * | Button | View Type | Modifier | Action |
 * | :----: | :-------: | :------: | ------ |
 * | Left   | Flat / 2D | None     | Update current coordinate under the mouse. |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     | None   |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | Middle | Flat / 2D | None     | Translate view in the XY plane, modifying the corresponding ranges of the x and y axes |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     |        |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | Right  | Flat / 2D | None     | None   |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     | Rotate view volume around its centroid. |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 */
void MildredWidget::mousePositionChanged(Qt3DInput::QMouseEvent *event)
{
    // Mouse is over the widget, so grab focus (for keyboard events)
    setFocus();

    // Check previous position
    if (lastMousePosition_.isNull())
    {
        lastMousePosition_ = QPoint(event->x(), event->y());
        return;
    }

    // Right button - Rotate scene volume (3D)
    if (event->buttons() & Qt3DInput::QMouseEvent::RightButton)
    {
        // Rotations only allowed for 3D view
        if (!flatView_)
        {
            viewRotationMatrix_ *=
                QQuaternion::fromEulerAngles(event->y() - lastMousePosition_.y(), event->x() - lastMousePosition_.x(), 0.0);
            sceneRootTransform_->setRotation(viewRotationMatrix_);
        }
    }

    // Middle button - translate axis ranges (2D)
    if (event->buttons() & Qt3DInput::QMouseEvent::MiddleButton)
    {
        if (flatView_)
        {
            xAxis_->shiftLimitsByPixels(lastMousePosition_.x() - event->x());
            yAxis_->shiftLimitsByPixels(event->y() - lastMousePosition_.y());
            updateTransforms();
        }
    }

    updateShaderParameters();

    lastMousePosition_ = QPoint(event->x(), event->y());

    if (flatView_)
    {
        const auto coords = toAxes2D(lastMousePosition_);
        emit mouseCoordChanged(coords);
        mouseCoordEntity_->setText(QString("%1 %2").arg(QString::number(coords.x()), QString::number(coords.y())));
        if (mouseCoordStyle_ == MouseCoordStyle::MouseAnchor)
        {
            mouseCoordEntity_->setEnabled(true);
            printf("%d %d\n", event->x(), event->y());
            mouseCoordEntity_->setAnchorPosition(
                // {
                //     float(event->x()), metrics_.displayVolumeExtent().y() - float(event->y()), 0.1
                // }
                {
                    float(event->x()) - metrics_.nMarginPixels(), metrics_.displayVolumeExtent().y() - float(event->y()) - metrics_.nMarginPixels(), 0.1
                }
            );
        }
    }
        
}

void MildredWidget::mouseButtonPressed(Qt3DInput::QMouseEvent *event) {}

void MildredWidget::mouseButtonReleased(Qt3DInput::QMouseEvent *event) {}

void MildredWidget::mouseWheeled(Qt3DInput::QWheelEvent *event)
{
    if (flatView_)
    {
        const auto sensitivity = 3;

        // Determine factor based on wheel amount, and axis deltas based on that
        auto sign = event->angleDelta().y() > 0 ? 1 : -1;
        const auto factor = 0.05;
        auto xDelta = xAxis_->range() * factor * sign;
        auto yDelta = yAxis_->range() * factor * sign;
        xAxis_->setLimits(xAxis_->minimum() + xDelta, xAxis_->maximum() - xDelta);
        yAxis_->setLimits(yAxis_->minimum() + yDelta, yAxis_->maximum() - yDelta);

        // Shift view centre towards current mouse position
        // -- Get the data-space delta between the centre coordinates of the 2D axes and the current mouse position
        auto centreDelta = QPoint(event->x(), height() - event->y()) - screen2DCentre();
        xAxis_->shiftLimitsByPixels(centreDelta.x() / (sign * sensitivity));
        yAxis_->shiftLimitsByPixels(centreDelta.y() / (sign * sensitivity));

        updateTransforms();
    }
}