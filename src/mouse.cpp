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
void MildredWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Check previous position
    if (lastMousePosition_.isNull())
    {
        lastMousePosition_ = event->position();
        return;
    }

    auto pos = event->position();
    auto needUpdate = false;

    // Right button - Rotate scene volume (3D)
    if (event->buttons().testFlag(Qt::RightButton))
    {
        // Rotations only allowed for 3D view
        if (!flatView_)
        {
            viewRotationMatrix_ *=
                QQuaternion::fromEulerAngles(pos.y() - lastMousePosition_.y(), pos.x() - lastMousePosition_.x(), 0.0);
            sceneRootTransform_->setRotation(viewRotationMatrix_);

            needUpdate = true;
        }
    }

    // Middle button - translate axis ranges (2D)
    if (event->buttons().testFlag(Qt::MiddleButton))
    {
        if (flatView_)
        {
            xAxis_->shiftLimits(-xAxis_->range() * ((pos.x() - lastMousePosition_.x()) / metrics_.displayVolumeExtent().x()));
            yAxis_->shiftLimits(yAxis_->range() * ((pos.y() - lastMousePosition_.y()) / metrics_.displayVolumeExtent().y()));

            needUpdate = true;
        }
    }

    if (needUpdate)
    {
        updateTransforms();
        updateShaderParameters();

        update();
    }

    lastMousePosition_ = pos;
}

// Mouse press event
void MildredWidget::mousePressEvent(QMouseEvent *event) { pressedMousePosition_ = event->pos(); }

// Mouse release event
void MildredWidget::mouseReleaseEvent(QMouseEvent *event) {}

// Mouse wheel event
void MildredWidget::wheelEvent(QWheelEvent *event) {}

// Mouse double click event
void MildredWidget::mouseDoubleClickEvent(QMouseEvent *event) {}