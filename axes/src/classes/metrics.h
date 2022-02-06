#pragma once

#include <QFont>
#include <QVector3D>

// Forward Declarations
class AxisEntity;

//! The MildredMetrics object contains basic view / layout metrics.
/*!
 * The @class MildredMetrics class controls the overall look and layout of the main plot area and axes, and core metrics
 * defining the volume extent and position. The volume extent along each of the cardinal directions is used by the various data
 * display classes in order to plot and scale the data correctly in 3D space.
 *
 * The metrics are frequently updated when objects within the view (e.g. axis titles, labels), or the size of the view surface
 * itself, are changed, so as to ensure that the graph area is correctly laid out and sized relative to the view.
 */
class MildredMetrics : public QObject
{
    Q_OBJECT

    /*
     * Fonts
     */
    private:
    // Fonts
    QFont axisTickLabelFont_{"Tahoma", 10}, axisTitleFont_{"Tahoma", 11};

    public:
    // Return font for axis tick labels
    const QFont &axisTickLabelFont() const;
    // Return font for axis title
    const QFont &axisTitleFont() const;

    /*
     * Axis
     */
    private:
    // Tick size
    float tickPixelSize_{5.0};
    // Spacing between tick marks and labels
    float tickLabelPixelGap_{0.0};

    public:
    // Return tick size
    float tickPixelSize() const;
    // Return spacing between tick marks and labels
    float tickLabelPixelGap() const;

    /*
     * Object Anchor Point
     */
    public:
    enum class AnchorPoint
    {
        TopLeft,
        TopMiddle,
        TopRight,
        MiddleLeft,
        Middle,
        MiddleRight,
        BottomLeft,
        BottomMiddle,
        BottomRight
    };
    // Return the fractional anchor location, with (0,0) corresponding to bottom-left corner
    static QPointF anchorLocation(AnchorPoint anchor);

    /*
     * Display
     */
    private:
    // Margin around outer edge of plot
    float nMarginPixels_{5.0};
    // Display volume origin
    QVector3D displayVolumeOrigin_;
    // Display volume bounds
    QVector3D displayVolumeExtent_;

    public:
    // Return margin around outer edge of plot
    float nMarginPixels() const;
    // Return display volume origin
    QVector3D displayVolumeOrigin() const;
    // Return display volume extent
    QVector3D displayVolumeExtent() const;

    /*
     * Update
     */
    public:
    // Update metrics given supplied surface size and axes
    void update(int width, int height, const AxisEntity *xAxis, const AxisEntity *yAxis);

    signals:
    void metricsChanged();
};
