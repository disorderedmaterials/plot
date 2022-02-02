#pragma once

#include "lineentity.h"
#include "metrics.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QExtrudedTextMesh>

class AxisEntity : public Qt3DCore::QEntity
{
    Q_OBJECT

    public:
    // Axis type
    enum class AxisType
    {
        Horizontal,
        Vertical,
        AltVertical,
        Depth,
        Custom
    };
    AxisEntity(AxisType type, Qt3DCore::QNode *parent = nullptr);
    ~AxisEntity() = default;

    /*
     * Definition
     */
    private:
    // Axis type
    AxisType type_;
    // Global scaling for components
    double axisScale_{100.0};
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
    std::vector<std::pair<double, bool>> generateLinearTicks();
    // Generate logarithmic ticks
    std::vector<std::pair<double, bool>> generateLogarithmicTicks();

    public:
    // Set axis type
    void setType(AxisType type);
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
    // Tick labels
    std::vector<std::tuple<Qt3DCore::QEntity *, Qt3DExtras::QExtrudedTextMesh *, Qt3DCore::QTransform *>> tickLabelMeshes_;

    private:
    // Create / update ticks and labels at specified axis values
    void createTicksAndLabels(const std::vector<std::pair<double, bool>> &ticks, const MildredMetrics &metrics);

    public:
    // Recreate axis
    void recreate(const MildredMetrics &metrics);
    // Add component to child entities
    void addComponentToChildren(Qt3DCore::QComponent *comp);
};
