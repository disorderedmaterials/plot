#pragma once

#include "classes/metrics.h"
#include "entities/axis.h"
#include "entities/line.h"
#include "entities/text.h"
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

namespace Mildred
{
// Forward Declarations
class RenderableMaterial;

//! AxisEntity completely defines and renders an axis.
/*!
 * AxisEntity defines a QEntity which represents a single axis, encompassing its numerical range and type and providing a
 * renderable representation in 3D. The entity contains @class LineEntity objects representing the axis bar, tick and sub-tick
 * marks, and @class TextEntity objects to render the tick value labels and axis title.
 */
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
    AxisEntity(Qt3DCore::QNode *parent, AxisType type, const MildredMetrics &metrics, RenderableMaterial *barMaterial,
               RenderableMaterial *labelMaterial);
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
    // Number of ticks to display
    int nSubTicks_{4};
    // Whether to determine major ticks automatically
    bool autoTicks_{true};

    private:
    // Calculate suitable tick start and delta
    std::pair<double, double> calculateTickStartAndDelta() const;
    // Generate linear ticks
    std::vector<std::pair<double, bool>> generateLinearTicks(double tickStart, double tickDelta) const;
    // Generate logarithmic ticks
    std::vector<std::pair<double, bool>> generateLogarithmicTicks() const;

    public:
    // Return the minimum display value of the axis
    double minimum() const;
    // Return the maximum display value of the axis
    double maximum() const;
    // Set limits of axis
    void setLimits(double minValue, double maxValue);
    // Return range of axis
    double range() const;
    // Shift limits of axis
    void shiftLimits(double delta);
    // Return whether the axis is logarithmic
    bool isLogarithmic() const;
    // Set title text
    void setTitleText(const QString &text);
    // Return title text
    QString titleText() const;

    public slots:
    // Set axis minimum
    void setMinimum(double value);
    // Set axis maximum
    void setMaximum(double value);
    // Set whether the axis is logarithmic
    void setLogarithmic(bool b);

    signals:
    void rangeChanged();

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
    void setDirection(QVector3D v);
    // Return explicit direction
    QVector3D direction() const;
    // Get relevant scale from the supplied metrics
    double getAxisScale(const MildredMetrics &metrics) const;
    // Map axis value to scaled global position
    double toGlobal(double axisValue) const;
    // Map axis value to 3D point
    QVector3D to3D(double axisValue) const;
    // Return scaled value point
    QVector3D toScaled(double axisValue) const;

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
    RenderableMaterial *axisBarMaterial_{nullptr}, *labelMaterial_{nullptr};

    public:
    // Return axis bar material
    RenderableMaterial *axisBarMaterial();
    // Return label material
    RenderableMaterial *labelMaterial();
};
} // namespace Mildred