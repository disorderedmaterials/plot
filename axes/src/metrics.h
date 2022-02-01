#pragma once

#include <QFont>

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
      * Display
      */
public:
     // Display volume bounds
     QVector3D displayVolumeOrigin, displayVolumeExtent;
};
