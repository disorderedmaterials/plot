#pragma once

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include "lineentity.h"

class AxisEntity : public Qt3DCore::QEntity
{
public:
    AxisEntity(Qt3DCore::QNode *parent = nullptr);
    ~AxisEntity() = default;

    /*
     * Definition
     */
    private:
    // Global scaling for components
    double axisScale_{10.0}, tickScale_{0.1};
    // Axis direction
    QVector3D direction_{1.0, 0.0, 0.0};
    // Tick direction
    QVector3D tickDirection_{0.0, -1.0, 0.0};
    // Extrema
    double minimum_{0.0}, maximum_{10.0};
    // Whether the axis is logarithmic
    bool logarithmic_{false};
    // Whether the axis is inverted
    bool inverted_{false};
    // Number of ticks to display
    int nSubTicks_{4};
    // Tick delta
    double tickDelta_{1.0};
    // Tick start value
    double tickStart_{0.0};
    // Whether to determine major ticks automatically
    bool autoTicks_{false};

private:
    // Calculate tick deltas
    void calculateTickDeltas();
    // Generate linear ticks
    void generateLinearTicks();
    // Generate logarithmic ticks
    void generateLogarithmicTicks();

public:
    // Set the global scale for the axis
    void setGlobalScale(double globalScale);
    // Define direction
    void setDirection(QVector3D principal);
    // Map axis value to scaled global position
    double axisToGlobal(double axisValue) const;

    /*
     * Entities
     */
private:
    // Axis bar
    LineEntity axisBarEntity_;
    // Tick marks
    LineEntity ticksEntity_, subTicksEntity_;

public:
    // Recreate axis
    void recreate();
    // Add component to child entities
    void addComponentToChildren(Qt3DCore::QComponent *comp);
};
