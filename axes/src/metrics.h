#pragma once

#include <QFont>
#include <QVector3D>

class MildredMetrics
{
    /*
     * General
     */
    public:
    // Display font
    QFont font;
    // Margin around outer edge of plot
    float nMarginPixels{2.0};

    /*
     * Axis
     */
    public:
    // Tick size
    float tickPixelSize{5.0};
    // Spacing between tick marks and labels
    float tickLabelPixelGap{4.0};

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
    public:
    // Display volume bounds
    QVector3D displayVolumeOrigin, displayVolumeExtent;
};
