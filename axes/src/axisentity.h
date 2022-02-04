#pragma once

#include "lineentity.h"
#include "metrics.h"
#include "textentity.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
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
    AxisEntity(AxisType type, const MildredMetrics &metrics, Qt3DCore::QNode *parent = nullptr);
    ~AxisEntity() = default;

    private:
    // Reference to chart metrics object
    const MildredMetrics &metrics_;

    /*
     * Definition
     */
    private:
    // Axis type
    AxisType type_;
    // Extrema
    double minimum_{0.0}, maximum_{10.0};
    // Whether the axis is logarithmic
    bool logarithmic_{false};
    // Whether the axis is inverted
    bool inverted_{false};
    // Number of ticks to display
    int nSubTicks_{4};
    // Whether to determine major ticks automatically
    bool autoTicks_{false};

    private:
    // Calculate suitable tick start and delta
    std::pair<double, double> calculateTickStartAndDelta() const;
    // Generate linear ticks
    std::vector<std::pair<double, bool>> generateLinearTicks(double tickStart, double tickDelta) const;
    // Generate logarithmic ticks
    std::vector<std::pair<double, bool>> generateLogarithmicTicks() const;

    public:
    // Set title text
    void setTitleText(const QString &title);
    // Return title text
    QString titleText() const;

    /*
     * Layout
     */
    private:
    // Global scaling for components
    double axisScale_{100.0};
    // Axis direction
    QVector3D direction_{1.0, 0.0, 0.0};
    // Axis direction array index (for easier manipulation of QVector3D)
    int axisDirectionIndex_{0};
    // Tick direction
    QVector3D tickDirection_{0.0, -1.0, 0.0};
    // Tick direction array index (for easier manipulation of QVector3D)
    int tickDirectionIndex_{0};
    // Tick label anchor point
    MildredMetrics::AnchorPoint labelAnchorPoint_{MildredMetrics::AnchorPoint::TopMiddle};

    public:
    // Set axis type
    void setType(AxisType type);
    // Set explicit direction
    void setDirection(QVector3D principal);
    // Get relevant scale from the supplied metrics
    double getAxisScale(const MildredMetrics &metrics) const;
    // Map axis value to scaled global position
    double axisToGlobal(double axisValue) const;

    /*
     * Entities
     */
    private:
    // Axis bar
    LineEntity *axisBarEntity_{nullptr};
    // Tick marks
    LineEntity *ticksEntity_{nullptr}, *subTicksEntity_{nullptr};
    // Tick labels
    std::vector<TextEntity *> tickLabelEntities_;
    // Axis title
    TextEntity *axisTitleEntity_{nullptr};

    private:
    // Create / update ticks and labels at specified axis values, returning their bounding cuboid
    Cuboid createTickAndLabelEntities(const std::vector<std::pair<double, bool>> &ticks);

    public:
    // Return bounding cuboid for axis given its current settings and supplied metrics
    Cuboid boundingCuboid(const MildredMetrics &metrics) const;

    public slots:
    // Recreate axis entities from scratch using stored metrics
    void recreate();

    /*
     * Components
     */
    private:
    // Materials
    Qt3DExtras::QDiffuseSpecularMaterial *axisBarMaterial_{nullptr}, *labelMaterial_{nullptr};

    public:
    // Return axis bar material
    Qt3DExtras::QDiffuseSpecularMaterial *axisBarMaterial();
    // Return label material
    Qt3DExtras::QDiffuseSpecularMaterial *labelMaterial();
};
