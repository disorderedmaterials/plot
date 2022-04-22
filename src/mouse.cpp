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
            xAxis_->shiftLimits(-xAxis_->range() *
                                (double(event->x() - lastMousePosition_.x()) / metrics_.displayVolumeExtent().x()));
            yAxis_->shiftLimits(yAxis_->range() *
                                (double(event->y() - lastMousePosition_.y()) / metrics_.displayVolumeExtent().y()));
            updateTransforms();
        }
    }

    updateShaderParameters();

    lastMousePosition_ = QPoint(event->x(), event->y());
}

void MildredWidget::mouseButtonPressed(Qt3DInput::QMouseEvent *event) {}

void MildredWidget::mouseButtonReleased(Qt3DInput::QMouseEvent *event) {}
