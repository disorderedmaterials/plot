#include "axisentity.h"

AxisEntity::AxisEntity(AxisType type, Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent), type_(type), axisBarEntity_(this), ticksEntity_(this, Qt3DRender::QGeometryRenderer::Lines),
      subTicksEntity_(this, Qt3DRender::QGeometryRenderer::Lines)
{
    setType(type_);
}

/*
 * Definition
 */

// Calculate tick deltas
void AxisEntity::calculateTickDeltas()
{
    const auto nBaseValues = 5, maxIterations = 10, maxTicks = 10;
    auto baseValueIndex = 0, minTicks = maxTicks / 2;
    int nTicks, iteration;
    auto baseValues = std::vector<int>{1, 2, 3, 4, 5};

    auto power = int(log10((maximum_ - minimum_) / maxTicks) - 1);
    iteration = 0;

    if ((maximum_ - minimum_) > 1.0e-10)
    {
        do
        {
            // Calculate current tickDelta
            tickDelta_ = baseValues[baseValueIndex] * pow(10.0, power);

            // Get first tickmark value
            tickStart_ = int(minimum_ / tickDelta_) * tickDelta_;

            // How many ticks now fit between the firstTick and max value?
            // Add 1 to get total ticks for this delta (i.e. including firstTick)
            nTicks = int((maximum_ - minimum_) / tickDelta_) + 1;

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
    }
    else
    {
        tickStart_ = minimum_;
        tickDelta_ = 1.0;
    }
}

// Generate linear ticks
std::vector<std::pair<double, bool>> AxisEntity::generateLinearTicks()
{
    // Calculate autoticks if requested
    if (autoTicks_)
        calculateTickDeltas();

    // Check tickDelta
    if (((maximum_ - minimum_) / tickDelta_) > 1e4)
        return {};

    // Plot tickmarks
    auto count = 0;
    auto delta = tickDelta_ / (nSubTicks_ + 1);
    auto value = tickStart_;
    std::vector<std::pair<double, bool>> ticks;
    while (value <= maximum_)
    {
        // Draw tick here, only if value >= minimum_
        if (value >= minimum_)
        {
            auto x = axisToGlobal(value);

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

// Generate logarithmic ticks
std::vector<std::pair<double, bool>> AxisEntity::generateLogarithmicTicks()
{
    // Check data range
    if (maximum_ < 0.0)
    {
        printf("Axis range is inappropriate for a log scale ({} < x < {}). Axis will not be drawn.\n", minimum_, maximum_);
        return {};
    }

    // Grab logged min/max values for convenience, enforcing sensible minimum
    auto min = log10(minimum_ <= 0.0 ? 1.0e-10 : minimum_);

    // Plot tickmarks - Start at floored (ceiling'd) integer of logAxisMin (logAxisMax), and go from there.
    auto nMinorTicks = std::min(nSubTicks_, 9);
    auto count = 0;
    auto power = floor(min);
    auto value = pow(10, power);
    auto u = 0.0;
    std::vector<std::pair<double, bool>> ticks;
    while (value <= maximum_)
    {
        // If the current value is in range, plot a tick
        u = (inverted_ ? log10(maximum_ / value) : log10(value)) * axisScale_;
        if (log10(value) >= min)
        {
            // Tick mark
            if (count == 0)
            {
                ticks.emplace_back(value, true);
            }
            else
                ticks.emplace_back(value, false);
        }

        // Increase tick counter, value, and power if necessary
        ++count;
        if (count == nMinorTicks)
        {
            count = 0;
            power = power + 1.0;
            value = pow(10, power);
        }
        else
            value += pow(10, power);
    }

    return ticks;
}

// Set axis type
void AxisEntity::setType(AxisType type)
{
    type_ = type;

    // Set default local metrics based on the axis type
    if (type_ == AxisType::X || type_ == AxisType::Custom)
    {
        direction_ = QVector3D(1.0f, 0.0f, 0.0f);
        tickDirection_ = QVector3D(0.0f, -1.0, 0.0f);
    }
    else if (type_ == AxisType::Y)
    {
        direction_ = QVector3D(0.0f, 1.0f, 0.0f);
        tickDirection_ = QVector3D(-1.0f, 0.0f, 0.0f);
    }
    else if (type_ == AxisType::AltY)
    {
        direction_ = QVector3D(0.0f, 1.0f, 0.0f);
        tickDirection_ = QVector3D(1.0f, 0.0f, 0.0f);
    }
    else if (type_ == AxisType::Z)
    {
        direction_ = QVector3D(0.0f, 0.0f, 1.0f);
        tickDirection_ = QVector3D(-1.0f, 0.0f, 0.0f);
    }
}

// Define direction
void AxisEntity::setDirection(QVector3D principal) { direction_ = principal; }

// Map axis value to scaled global position
double AxisEntity::axisToGlobal(double axisValue) const
{
    if (logarithmic_)
        return ((axisValue - minimum_) / (maximum_ - minimum_)) * axisScale_;
    else
        return ((inverted_ ? (maximum_ - axisValue) : (axisValue - minimum_)) / (maximum_ - minimum_)) * axisScale_;
}

/*
 * Entities
 */

// Create / update tick labels at specified axis values
void AxisEntity::createTicksAndLabels(const std::vector<std::pair<double, bool>> &ticks, const MildredMetrics &metrics)
{
    // First, hide all existing label entities
    for (auto &&[entity, mesh, transform] : tickLabelMeshes_)
        entity->setEnabled(false);

    // Determine number of labels required
    auto nTicks = std::count_if(ticks.begin(), ticks.end(), [](const auto &t) { return t.second; });

    // Create any new text entities that we need
    while (tickLabelMeshes_.size() < nTicks)
    {
        auto *entity = new Qt3DCore::QEntity(this);
        auto *mesh = new Qt3DExtras::QExtrudedTextMesh(this);
        auto *transform = new Qt3DCore::QTransform(entity);
        mesh->setFont(metrics.font);
        mesh->setDepth(0.1);
        entity->addComponent(mesh);
        transform->setScale3D({float(metrics.font.pointSizeF()), float(metrics.font.pointSizeF()), 1.0});
        entity->addComponent(transform);
        tickLabelMeshes_.emplace_back(entity, mesh, transform);
    }

    // Loop over new values and create / update entities as required
    // TODO Missing our zip operator here!
    auto n = 0;
    for (auto &&[v, label] : ticks)
    {
        auto vT = float(axisToGlobal(v));

        if (label)
        {
            // Create tick mark
            ticksEntity_.addVertices({{direction_ * vT}, {direction_ * vT + tickDirection_ * metrics.tickPixelSize}});

            // Set label details
            auto &&[entity, mesh, transform] = tickLabelMeshes_[n];
            entity->setEnabled(true);
            mesh->setText(QString::number(v));
            transform->setTranslation({vT, -(float(metrics.font.pointSizeF()) + metrics.tickPixelSize), 0.0});
            ++n;
        }
        else
            subTicksEntity_.addVertices({{direction_ * vT}, {direction_ * vT + tickDirection_ * metrics.tickPixelSize * 0.5}});
    }
}

void AxisEntity::recreate(const MildredMetrics &metrics)
{
    if (type_ == AxisType::X)
        axisScale_ = metrics.displayVolumeExtent.x();
    else if (type_ == AxisType::Y || type_ == AxisType::AltY)
        axisScale_ = metrics.displayVolumeExtent.y();
    else if (type_ == AxisType::Y)
        axisScale_ = metrics.displayVolumeExtent.z();

    // Clear old primitives
    axisBarEntity_.clear();
    ticksEntity_.clear();
    subTicksEntity_.clear();

    // Plot basic axis line
    axisBarEntity_.addVertices({{0.0, 0.0, 0.0}, direction_ * float(axisScale_)});
    axisBarEntity_.setBasicIndices();
    axisBarEntity_.finalise();

    // Generate axis ticks
    auto ticks = logarithmic_ ? generateLogarithmicTicks() : generateLinearTicks();
    createTicksAndLabels(ticks, metrics);

    ticksEntity_.setBasicIndices();
    ticksEntity_.finalise();
    subTicksEntity_.setBasicIndices();
    subTicksEntity_.finalise();
}

// Add component to child entities
void AxisEntity::addComponentToChildren(Qt3DCore::QComponent *comp)
{
    axisBarEntity_.addComponent(comp);
    ticksEntity_.addComponent(comp);
    subTicksEntity_.addComponent(comp);
}
