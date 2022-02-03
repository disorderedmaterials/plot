#include "metrics.h"

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
