#include "metrics.h"
#include "axisentity.h"

/*
 * Fonts
 */

// Return font for axis tick labels
const QFont &MildredMetrics::axisTickLabelFont() const { return axisTickLabelFont_; }

// Return font for axis title
const QFont &MildredMetrics::axisTitleFont() const { return axisTitleFont_; }

/*
 * Axis
 */

// Return tick size
float MildredMetrics::tickPixelSize() const { return tickPixelSize_; }

// Return spacing between tick marks and labels
float MildredMetrics::tickLabelPixelGap() const { return tickLabelPixelGap_; }

/*
 * Object Anchor Point
 */

// Return the fractional anchor location, with (0,0) corresponding to bottom-left corner
QPointF MildredMetrics::anchorLocation(AnchorPoint anchor)
{
    switch (anchor)
    {
        case (MildredMetrics::AnchorPoint::TopLeft):
            return {0.0, 1.0};
        case (MildredMetrics::AnchorPoint::TopMiddle):
            return {0.5, 1.0};
        case (MildredMetrics::AnchorPoint::TopRight):
            return {1.0, 1.0};
        case (MildredMetrics::AnchorPoint::MiddleLeft):
            return {0.0, 0.5};
        case (MildredMetrics::AnchorPoint::Middle):
            return {0.5, 0.5};
        case (MildredMetrics::AnchorPoint::MiddleRight):
            return {1.0, 0.5};
        case (MildredMetrics::AnchorPoint::BottomLeft):
            return {0.0, 0.0};
        case (MildredMetrics::AnchorPoint::BottomMiddle):
            return {0.5, 0.0};
        case (MildredMetrics::AnchorPoint::BottomRight):
            return {1.0, 0.0};
        default:
            throw(std::runtime_error("Unhandled anchor point."));
    }
}

/*
 * Display
 */

// Return margin around outer edge of plot
float MildredMetrics::nMarginPixels() const { return nMarginPixels_; }

// Return display volume origin
QVector3D MildredMetrics::displayVolumeOrigin() const { return displayVolumeOrigin_; }

// Return display volume bounds
QVector3D MildredMetrics::displayVolumeExtent() const { return displayVolumeExtent_; }

/*
 * Update
 */

// Update metrics given supplied surface size and axes
void MildredMetrics::update(int width, int height, const AxisEntity *xAxis, const AxisEntity *yAxis)
{
    // Initialise the display volume origin to zero (pixels) and the extent to the max surface size
    displayVolumeOrigin_ = QVector3D(0, 0, 0);
    displayVolumeExtent_ = QVector3D(float(width), float(height), float(width + height) / 2.0f);

    // Apply margin around extreme edge of surface
    displayVolumeOrigin_ += QVector3D(nMarginPixels_, nMarginPixels_, 0.0);
    displayVolumeExtent_ -= QVector3D(2.0f * nMarginPixels_, 2.0f * nMarginPixels_, 0.0);

    // Reduce display volume to accommodate axes
    if (xAxis && xAxis->isEnabled())
    {
        auto cuboid = xAxis->boundingCuboid(*this);
        displayVolumeOrigin_ += QVector3D(0.0, cuboid.yExtent(), 0.0);
        displayVolumeExtent_ -= QVector3D(0.0, cuboid.yExtent(), 0.0);
    }
    if (yAxis && yAxis->isEnabled())
    {
        auto cuboid = yAxis->boundingCuboid(*this);
        displayVolumeOrigin_ += QVector3D(cuboid.xExtent(), 0.0, 0.0);
        displayVolumeExtent_ -= QVector3D(cuboid.xExtent(), 0.0, 0.0);
    }

    emit(metricsChanged());
}
