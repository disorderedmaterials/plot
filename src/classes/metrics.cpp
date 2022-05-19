#include "classes/metrics.h"
#include "entities/axis.h"
#include <stdexcept>

using namespace Mildred;

/*
 * Fonts
 */

//! Return font for axis tick labels
const QFont &MildredMetrics::axisTickLabelFont() const { return axisTickLabelFont_; }

//! Return font for axis title
const QFont &MildredMetrics::axisTitleFont() const { return axisTitleFont_; }

/*
 * Axis
 */

//! Return tick size in pixels.
/*!
 * Returns the length, in pixels, of axis tick marks. Note that this directly defines the length of tick marks at labelled
 * positions - sub (minor) ticks in between labelled value positions are drawn with half the length defined here.
 */
float MildredMetrics::tickPixelSize() const { return tickPixelSize_; }

//! Return spacing between tick marks and labels.
/*!
 * Returns the spacing, in pixels, to add between tick marks at labelled positions and tick label values.
 */
float MildredMetrics::tickLabelPixelGap() const { return tickLabelPixelGap_; }

/*
 * Object Anchor Point
 */

//! Return fractional anchor position.
/*!
 * Return the fractional coordinate of the specified anchor location in the XY plane, for use when positioning other objects.
 *
 * (0,0) corresponds to the bottom-left corner, and (1,1) corresponds the the top-right.
 */
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

// Return whether the anchor point is primarily associated with vertical edges
bool MildredMetrics::isAnchorPointVertical(MildredMetrics::AnchorPoint anchor)
{
    return (anchor == AnchorPoint::MiddleLeft || anchor == AnchorPoint::Middle || anchor == AnchorPoint::MiddleRight);
}

/*
 * Display
 */

//! Return margin around outer edge of plot
/*!
 * Returns the margin, in pixels, to reserve around the extreme edge of the display surface.
 */
float MildredMetrics::nMarginPixels() const { return nMarginPixels_; }

//! Return display volume origin
/*!
 * Return the calculated display volume origin coordinate. This is the coordinate, in 3D space, by which an entity must be
 * transformed in order to assure that drawing at (0,0,0) coincides with the junction point of the displayed axes objects.
 */
QVector3D MildredMetrics::displayVolumeOrigin() const { return displayVolumeOrigin_; }

//! Return display volume extent
/*!
 * Returns the calculated available display volume extents, in units of pixels, available for plotting data series. The extents
 * define the maximal distance from the display volume origin that is available for plotting.
 */
QVector3D MildredMetrics::displayVolumeExtent() const { return displayVolumeExtent_; }

/*
 * Update
 */

//! Update metrics given supplied surface size and axes
/*!
 * Recalculates the available display origin and extents given the supplied pixel @param width and @param height of the drawing
 * surface, and the current @param xAxis and @param yAxis entities.
 *
 * Emits metricsChanged().
 */
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
