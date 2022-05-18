#include "entities/axis.h"
#include "classes/cuboid.h"
#include "material.h"
#include <stdexcept>

using namespace Mildred;

//! Create a new AxisEntity
/*!
 * Constructs a new AxisEntity of the specified @param type, and referencing the supplied @class MildredMetrics.
 *
 * The direction of the axis bar and tick marks in 3D space is set from the given @param type, as are the anchor point and
 * orientation of the axis title.
 */
AxisEntity::AxisEntity(Qt3DCore::QNode *parent, AxisType type, const MildredMetrics &metrics, RenderableMaterial *barMaterial,
                       RenderableMaterial *labelMaterial)
    : Qt3DCore::QEntity(parent), metrics_(metrics), type_(type)
{
    assert(barMaterial);
    assert(labelMaterial);

    // Create entities
    axisBarEntity_ = new LineEntity(this);
    ticksEntity_ = new LineEntity(this, Qt3DRender::QGeometryRenderer::Lines);
    subTicksEntity_ = new LineEntity(this, Qt3DRender::QGeometryRenderer::Lines);
    axisTitleEntity_ = new TextEntity(this, "Unnamed Axis");
    axisTitleEntity_->setAnchorPoint(MildredMetrics::AnchorPoint::TopMiddle);

    // Assign material components
    axisBarMaterial_ = barMaterial;
    axisBarEntity_->addComponent(axisBarMaterial_);
    ticksEntity_->addComponent(axisBarMaterial_);
    subTicksEntity_->addComponent(axisBarMaterial_);
    labelMaterial_ = labelMaterial;
    axisTitleEntity_->setMaterial(labelMaterial_);

    // Update data dependent on the axis type
    setType(type_);

    // Connect to metrics object
    connect(&metrics_, SIGNAL(metricsChanged()), this, SLOT(recreate()));
}

/*
 * Definition
 */

//! Calculate suitable tick start and delta
/*!
 * Calculate and return a suitable tick start and delta for the axis given the current range.
 */
std::pair<double, double> AxisEntity::calculateTickStartAndDelta() const
{
    // Constants
    const auto nBaseValues = 5, maxIterations = 10, maxTicks = 10;
    const auto baseValues = std::vector<int>{1, 2, 3, 4, 5};

    auto baseValueIndex = 0, minTicks = maxTicks / 2;
    int nTicks, iteration;

    auto power = int(log10((maximum_ - minimum_) / maxTicks) - 1);
    iteration = 0;

    // TODO This is a poorly-formed check - what if the axis spans values lower than 1.0e-10?
    if ((maximum_ - minimum_) <= 1.0e-10)
        return {minimum_, 1.0};

    auto tickStart = 0.0, tickDelta = 0.0;
    do
    {
        // Calculate current tickDelta
        tickDelta = baseValues[baseValueIndex] * pow(10.0, power);

        // Get first tickmark value
        tickStart = int(minimum_ / tickDelta) * tickDelta;

        // How many ticks now fit between the firstTick and max value?
        // Add 1 to get total ticks for this delta (i.e. including firstTick)
        nTicks = int((maximum_ - minimum_) / tickDelta) + 1;

        // Check n...
        if (nTicks > maxTicks)
        {
            ++baseValueIndex;
            if (baseValueIndex == nBaseValues)
                ++power;
            baseValueIndex = baseValueIndex % nBaseValues;
        }
        else if (nTicks < minTicks)
        {
            --baseValueIndex;
            if (baseValueIndex == -1)
            {
                --power;
                baseValueIndex += nBaseValues;
            }
        }

        ++iteration;
        if (iteration == maxIterations)
            break;

    } while ((nTicks > maxTicks) || (nTicks < minTicks));
    return {tickStart, tickDelta};
}

//! Generate linear ticks
/*!
 * Generate a series of evenly-spaced tick positions given the supplied @param tickStart and @param tickDelta. The returned
 * vector consists of pairs of double and bool specifying the numerical value (within the defined axis range) and whether that
 * tick is a full/value tick requiring a label (true) or is just a sub-tick (false).
 */
std::vector<std::pair<double, bool>> AxisEntity::generateLinearTicks(double tickStart, double tickDelta) const
{
    // Check tickDelta
    if (((maximum_ - minimum_) / tickDelta) > 100)
        return {};

    auto count = 0;
    auto delta = tickDelta / (nSubTicks_ + 1);
    auto value = tickStart - delta;
    std::vector<std::pair<double, bool>> ticks;

    // Go backwards from the first major tick, filling in minor ticks.
    while (value >= minimum_)
    {
        auto x = toGlobal(value);
        ticks.emplace_back(value, false);
        value -= delta;
    }

    value = tickStart;

    while (value <= maximum_)
    {
        // Add tick here, only if value >= minimum_
        if (value >= minimum_)
        {
            auto x = toGlobal(value);

            if (count % (nSubTicks_ + 1) == 0)
            {
                ticks.emplace_back(value, true);

                count = 0;
            }
            else
                ticks.emplace_back(value, false);
        }

        value += delta;
        ++count;
    }

    return ticks;
}

//! Generate logarithmic ticks
/*!
 * Generate a series of logarithmically-spaced tick positions given the supplied @param tickStart and @param tickDelta. The
 * returned vector consists of pairs of double and bool specifying the numerical value (within the defined axis range) and
 * whether that tick is a full/value tick requiring a label (true) or is just a sub-tick (false).
 */
std::vector<std::pair<double, bool>> AxisEntity::generateLogarithmicTicks() const
{
    // Check data range
    if (maximum_ < 0.0)
    {
        printf("Axis range is inappropriate for a log scale (%g < x < %g). Axis will not be drawn.\n", minimum_, maximum_);
        return {};
    }

    // Break up axis maximum into mantissa and exponent and set initial mantissa and power
    const auto exponent = floor(log10(fabs(maximum_) + std::numeric_limits<double>::min()));
    auto power = exponent == floor(log10(std::numeric_limits<double>::min())) ? 0 : int(exponent);
    const auto realMantissa = maximum_ / pow(10.0, power);
    auto intMantissa = 0;
    if (realMantissa >= 10.0)
    {
        intMantissa = int(realMantissa / 10.0);
        ++power;
    }
    else
        intMantissa = int(realMantissa);

    auto value = intMantissa * pow(10.0, power);

    std::vector<std::pair<double, bool>> ticks;
    while (value > minimum_)
    {
        // Generate a tick
        ticks.emplace_back(value, intMantissa == 1);

        // Step backwards
        --intMantissa;
        if (intMantissa == 0)
        {
            intMantissa = 9;
            --power;
        }

        // Calculate new value
        value = intMantissa * pow(10.0, power);
    }

    return ticks;
}

// Return the minimum display value of the axis
double AxisEntity::minimum() const { return minimum_; }

// Return the maximum display value of the axis
double AxisEntity::maximum() const { return maximum_; }

// Set limits of axis
void AxisEntity::setLimits(double minValue, double maxValue)
{
    // Disallow negative / zero values if we are logarithmic
    if (logarithmic_ && (minValue <= 0.0 || maxValue <= 0.0))
    {
        printf("Ignoring new limits (%e, %e) for axis as they are invalid for a logarithmic scale.\n", minValue, maxValue);
        return;
    }

    minimum_ = std::min(minValue, maxValue);
    maximum_ = std::max(minValue, maxValue);

    recreate();

    emit(rangeChanged());
}

//! Return range of axis
/*!
 * Return the value range of the axis, i.e. the numerical difference between the minimum and maximum values.
 */
double AxisEntity::range() const { return maximum_ - minimum_; }

//! Shift limits by axis delta specified
/*!
 * Adjust the limits of the axis by the supplied @param delta. The @param delta is added to both the minimum and maximum values.
 * The overall range of the axis is not modified, unless the axis represents a logarithmic scale.
 *
 * Emits rangeChanged() if the axis style is logarithmic.
 */
void AxisEntity::shiftLimits(double delta)
{
    if (logarithmic_)
    {
        auto logRange = log10(maximum_) - log10(minimum_);
        auto logDelta = logRange * (delta / range());
        minimum_ = pow(10.0, log10(minimum_) + logDelta);
        maximum_ = pow(10.0, log10(maximum_) + logDelta);

        rangeChanged();
    }
    else
    {
        minimum_ += delta;
        maximum_ += delta;
    }

    recreate();
}

//! Shift the limits of the axis based on the supplied screen pixel delta
/*!
 * Adjust the limits of the axis by the supplied @param pixelDelta. The @param pixelDelta is converted to a suitable axis value
 * delta which is then added to both the minimum and maximum values. The overall range of the axis is not modified, unless the
 * axis represents a logarithmic scale.
 *
 * Emits rangeChanged() if the axis style is logarithmic.
 */
// Shift limits of axis using the specified pixel delta
void AxisEntity::shiftLimitsByPixels(int pixelDelta)
{
    if (logarithmic_)
    {
        // Get logarithmic range and determine delta
        auto logRange = log10(maximum_) - log10(minimum_);
        auto delta = logRange * (double(pixelDelta) / axisScale_);

        // If we are logarithmic and the shift would cause a negative minimum_, ignore the request
        if (pow(10.0, log10(minimum_) + delta) <= 0.0)
            return;

        minimum_ = pow(10.0, log10(minimum_) + delta);
        maximum_ = pow(10.0, log10(maximum_) + delta);

        rangeChanged();
    }
    else
    {
        auto delta = range() * (double(pixelDelta) / axisScale_);

        minimum_ += delta;
        maximum_ += delta;
    }

    recreate();
}

//! Return whether the axis is logarithmic
bool AxisEntity::isLogarithmic() const { return logarithmic_; }

//! Set title text
/*!
 * Sets the display title of the axis to @param text, modifying the underlying @class TextEntity object.
 */
void AxisEntity::setTitleText(const QString &text)
{
    assert(axisTitleEntity_);
    axisTitleEntity_->setText(text);
}

//! Return title text
/*!
 * Returns the current text displayed as the axis title.
 */
QString AxisEntity::titleText() const
{
    assert(axisTitleEntity_);
    return axisTitleEntity_->text();
}

//! Set axis minimum
/*!
 * Set the minimum displayed limit of the axis to @param value. If the new minimum is greater than the current maximum the
 * limiting values are swapped to maintain the condition minimum < maximum.
 *
 * Once the new limit(s) are set, the axis entities are recreated.
 *
 * Emits rangeChanged().
 */
void AxisEntity::setMinimum(double value)
{
    // Disallow negative / zero values if we are logarithmic
    if (logarithmic_ && (value <= 0.0))
    {
        printf("Ignoring new minimum limit (%e) for axis as it is invalid for a logarithmic scale.\n", value);
        return;
    }

    // Switch maximum and minimum if the supplied minimum is greater than the current maximum
    if (value > maximum_)
    {
        minimum_ = maximum_;
        maximum_ = value;
    }
    else
        minimum_ = value;

    emit(rangeChanged());
}

//! Set axis maximum
/*!
 * Set the maximum displayed limit of the axis to @param value. If the new maximum is less than the current minimum the limiting
 * values are swapped to maintain the condition maximum > minimum.
 *
 * Once the new limit(s) are set, the axis entities are recreated.
 *
 * Emits rangeChanged().
 */
void AxisEntity::setMaximum(double value)
{
    // Disallow negative / zero values if we are logarithmic
    if (logarithmic_ && (value <= 0.0))
    {
        printf("Ignoring new maximum limit (%e) for axis as it is invalid for a logarithmic scale.\n", value);
        return;
    }

    // Switch maximum and minimum if the supplied maximum is less than the current minimum
    if (value < minimum_)
    {
        maximum_ = minimum_;
        minimum_ = value;
    }
    else
        maximum_ = value;

    recreate();

    emit(rangeChanged());
}

//! Set whether the axis is logarithmic
/*!
 * Set whether the axis is linear (false) or logarithmic (true). When changing to logarithmic a light check on the current
 * limits is made to ensure that display in a log scale is possible.
 *
 * If the style of axis is changed, the axis entities are recreated.
 *
 * Emits rangeChanged().
 */
void AxisEntity::setLogarithmic(bool b)
{
    if (logarithmic_ == b)
        return;

    logarithmic_ = b;

    // Check limits when switching to log axis
    if (logarithmic_)
    {
        if (maximum_ <= 0.0)
        {
            minimum_ = 0.1;
            maximum_ = 10.0;
        }
        else if (minimum_ <= 0.0)
            minimum_ = maximum_ / 100.0;
    }

    recreate();

    emit(rangeChanged());
}

/*
 * Layout
 */

//! Set axis type
/*!
 * Set the axis to be of the defined @param type, setting directional parameters (in 3D space) for the axis bar, tick marks and
 * label positioning.
 */
void AxisEntity::setType(AxisType type)
{
    type_ = type;

    // Set default local metrics based on the axis type
    if (type_ == AxisType::Horizontal || type_ == AxisType::Custom)
    {
        direction_ = QVector3D(1.0f, 0.0f, 0.0f);
        axisDirectionIndex_ = 0;
        tickDirection_ = QVector3D(0.0f, -1.0, 0.0f);
        tickDirectionIndex_ = 1;
        labelAnchorPoint_ = MildredMetrics::AnchorPoint::TopMiddle;
    }
    else if (type_ == AxisType::Vertical)
    {
        direction_ = QVector3D(0.0f, 1.0f, 0.0f);
        axisDirectionIndex_ = 1;
        tickDirection_ = QVector3D(-1.0f, 0.0f, 0.0f);
        tickDirectionIndex_ = 0;
        labelAnchorPoint_ = MildredMetrics::AnchorPoint::MiddleRight;
    }
    else if (type_ == AxisType::AltVertical)
    {
        direction_ = QVector3D(0.0f, 1.0f, 0.0f);
        axisDirectionIndex_ = 1;
        tickDirection_ = QVector3D(1.0f, 0.0f, 0.0f);
        tickDirectionIndex_ = 0;
        labelAnchorPoint_ = MildredMetrics::AnchorPoint::MiddleLeft;
    }
    else if (type_ == AxisType::Depth)
    {
        direction_ = QVector3D(0.0f, 0.0f, -1.0f);
        axisDirectionIndex_ = 2;
        tickDirection_ = QVector3D(-1.0f, 0.0f, 0.0f);
        tickDirectionIndex_ = 0;
        labelAnchorPoint_ = MildredMetrics::AnchorPoint::MiddleRight;
    }
}

//! Define direction
/*!
 * Set the direction of the axis in 3D space to the supplied vector @param v.
 */
void AxisEntity::setDirection(QVector3D v) { direction_ = v; }

// Return explicit direction
QVector3D AxisEntity::direction() const { return direction_; }

//! Get relevant scale from the supplied metrics
/*!
 * Retrieve the relevant scale from the supplied @param metrics given the current axis type.
 */
double AxisEntity::getAxisScale(const MildredMetrics &metrics) const
{
    if (type_ == AxisType::Horizontal)
        return metrics.displayVolumeExtent().x();
    else if (type_ == AxisType::Vertical || type_ == AxisType::AltVertical)
        return metrics.displayVolumeExtent().y();
    else if (type_ == AxisType::Depth)
        return metrics.displayVolumeExtent().z();

    throw(std::runtime_error("Return of scale for axis type not accounted for.\n"));
}

//! Map axis value to scaled global position
/*!
 * Convert the supplied @param axisValue into scaled view volume coordinates.
 */
double AxisEntity::toGlobal(double axisValue) const
{
    if (logarithmic_)
        return ((log10(axisValue) - log10(minimum_)) / (log10(maximum_) - log10(minimum_))) * axisScale_;
    else
        return ((axisValue - minimum_) / (maximum_ - minimum_)) * axisScale_;
}

//! Map axis value to 3D point
/*!
 * Convert the supplied @param axisValue into view volume coordinates along the direction of the axis.
 */
QVector3D AxisEntity::to3D(double axisValue) const { return direction_ * float(toGlobal(axisValue)); }

//! Return scaled value point
QVector3D AxisEntity::toScaled(double axisValue) const
{
    if (logarithmic_)
        return direction_ * log10(axisValue) * axisScale_ / (log10(maximum_) - log10(minimum_));
    else
        return direction_ * axisValue * axisScale_ / range();
}

//! Return axis value from scaled point
/*!
 * Convert the supplied pixel offset @param scaledValue into axis coordinates along the axis.
 */
double AxisEntity::fromScaled(double scaledValue) const
{
    if (logarithmic_)
        return pow(10.0, log10(minimum_) + (scaledValue / axisScale_) * (log10(maximum_) - log10(minimum_)));
    else
        return minimum_ + scaledValue * range() / axisScale_;
}

/*
 * Entities
 */

//! Create / update tick and label entities at specified axis values
/*!
 * Construct rendering entities displaying tick marks and labels from the supplied vector of @param ticks.
 *
 * During construction the bounding @class Cuboid containing the ticks and label entities is calculated and returned.
 */
Cuboid AxisEntity::createTickAndLabelEntities(const std::vector<std::pair<double, bool>> &ticks)
{
    // First, hide all existing label entities
    for (auto &entity : tickLabelEntities_)
        entity->setEnabled(false);

    // Determine number of labels required
    auto nTicks = std::count_if(ticks.begin(), ticks.end(), [](const auto &t) { return t.second; });

    // Create any new text entities that we need
    while (tickLabelEntities_.size() < nTicks)
    {
        auto *entity = new TextEntity(this);
        entity->setFont(metrics_.axisTickLabelFont());
        entity->setMaterial(labelMaterial_);
        entity->setAnchorPoint(labelAnchorPoint_);
        tickLabelEntities_.push_back(entity);
    }

    // Loop over new values and create / update entities as required
    Cuboid boundingCuboid;
    auto tickLabelEntity = tickLabelEntities_.begin();
    auto tickPos = tickDirection_ * metrics_.tickPixelSize();
    for (auto &&[v, label] : ticks)
    {
        auto axisPos = to3D(v);

        if (label)
        {
            // Create tick mark
            ticksEntity_->addVertices({axisPos, axisPos + tickPos});
            boundingCuboid.expand({axisPos, axisPos + tickPos});

            // Set label details
            (*tickLabelEntity)->setEnabled(true);
            (*tickLabelEntity)->setText(QString::number(v));
            (*tickLabelEntity)
                ->setAnchorPosition(axisPos + tickDirection_ * (metrics_.tickPixelSize() + metrics_.tickLabelPixelGap()));
            boundingCuboid.expand(TextEntity::boundingCuboid(
                metrics_.axisTickLabelFont(), QString::number(v),
                {axisPos + tickDirection_ * (metrics_.tickPixelSize() + metrics_.tickLabelPixelGap())}, labelAnchorPoint_));
            ++tickLabelEntity;
        }
        else
        {
            subTicksEntity_->addVertices({axisPos, {axisPos + tickPos * 0.5}});
            boundingCuboid.expand({axisPos, axisPos + tickPos * 0.5});
        }
    }

    return boundingCuboid;
}

//! Recreate axis entities from scratch using stored metrics
/*!
 * Recreate all entities for the axis from stored information. This function is called whenever the underlying @class
 * MildredMetrics object is changed (via Qt signalling) and is used internally when the axis limits are modified in some way.
 */
void AxisEntity::recreate()
{
    if (!isEnabled())
        return;

    // Store axis scale from metrics
    axisScale_ = getAxisScale(metrics_);

    // Clear old primitives
    axisBarEntity_->clear();
    ticksEntity_->clear();
    subTicksEntity_->clear();

    // Plot basic axis line
    axisBarEntity_->addVertices({{0.0, 0.0, 0.0}, direction_ * float(axisScale_)});
    axisBarEntity_->setBasicIndices();
    axisBarEntity_->finalise();

    // Generate axis ticks
    std::vector<std::pair<double, bool>> ticks;
    if (logarithmic_)
        ticks = generateLogarithmicTicks();
    else
    {
        // Calculate autoticks if requested
        if (autoTicks_)
        {
            auto [tickStart, tickDelta] = calculateTickStartAndDelta();
            ticks = generateLinearTicks(tickStart, tickDelta);
        }
        else
            ticks = generateLinearTicks(0.0, 1.0);
    }
    auto tickLabelBounds = createTickAndLabelEntities(ticks);

    // Axis title
    if (!axisTitleEntity_->text().isEmpty())
    {
        axisTitleEntity_->setEnabled(true);
        axisTitleEntity_->setFont(metrics_.axisTitleFont());
        axisTitleEntity_->setAnchorPoint(labelAnchorPoint_);
        axisTitleEntity_->setAnchorPosition(
            direction_ * metrics_.displayVolumeExtent()[axisDirectionIndex_] * 0.5 +
            tickDirection_ * (tickLabelBounds.extents()[tickDirectionIndex_] + metrics_.tickLabelPixelGap()));
    }
    else
        axisTitleEntity_->setEnabled(false);

    ticksEntity_->setBasicIndices();
    ticksEntity_->finalise();
    subTicksEntity_->setBasicIndices();
    subTicksEntity_->finalise();
}

//! Return bounding cuboid for axis given its current settings and supplied metrics
/*!
 * Return the bounding cuboid for the axis given the supplied @param metrics. The returned cuboid represents the limiting 3D
 * coordinates required to exactly enclose the axis bar, ticks, tick labels, and axis title.
 */
Cuboid AxisEntity::boundingCuboid(const MildredMetrics &metrics) const
{
    // Generate axis ticks
    std::vector<std::pair<double, bool>> ticks;
    if (logarithmic_)
        ticks = generateLogarithmicTicks();
    else
    {
        // Calculate autoticks if requested
        if (autoTicks_)
        {
            auto [tickStart, tickDelta] = calculateTickStartAndDelta();
            ticks = generateLinearTicks(tickStart, tickDelta);
        }
        else
            ticks = generateLinearTicks(0.0, 10.0);
    }

    // Determine bounding cuboid for the axis
    Cuboid cuboid;
    // -- Axis bar
    auto axisScale = getAxisScale(metrics);
    cuboid.expand({{0.0, 0.0, 0.0}, direction_ * float(axisScale)});

    // -- Ticks
    for (auto &&[v, label] : ticks)
    {
        auto axisPos = to3D(v);

        if (label)
        {
            // Tick mark
            cuboid.expand({axisPos, axisPos + tickDirection_ * metrics_.tickPixelSize()});

            // Label
            cuboid.expand(TextEntity::boundingCuboid(
                metrics.axisTickLabelFont(), QString::number(v),
                axisPos + tickDirection_ * (metrics_.tickPixelSize() + metrics.tickLabelPixelGap()), labelAnchorPoint_));
        }
        else
            cuboid.expand({axisPos, axisPos + tickDirection_ * metrics_.tickPixelSize() * 0.5});
    }
    // -- Title
    if (!axisTitleEntity_->text().isEmpty())
        cuboid.expand(TextEntity::boundingCuboid(metrics.axisTitleFont(), axisTitleEntity_->text(),
                                                 direction_ * metrics.displayVolumeExtent()[axisDirectionIndex_] * 0.5 +
                                                     tickDirection_ *
                                                         (cuboid.extents()[tickDirectionIndex_] + metrics.tickLabelPixelGap()),
                                                 labelAnchorPoint_));

    return cuboid;
}

/*
 * Components
 */

//! Return axis bar material
/*!
 * Return the material used for the axis bar.
 */
RenderableMaterial *AxisEntity::axisBarMaterial() { return axisBarMaterial_; }

//! Return label material
/*!
 * Return the material used for axis labels (tick value labels and axis title).
 */
RenderableMaterial *AxisEntity::labelMaterial() { return labelMaterial_; }
