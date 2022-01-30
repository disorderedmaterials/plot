#include "axisentity.h"

AxisEntity::AxisEntity(Qt3DCore::QNode *parent) : Qt3DCore::QEntity(parent), axisBarEntity_(this), ticksEntity_(this, Qt3DRender::QGeometryRenderer::Lines), subTicksEntity_(this, Qt3DRender::QGeometryRenderer::Lines)
{
    recreate();
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
void AxisEntity::generateLinearTicks()
{
    // Calculate autoticks if requested
    if (autoTicks_)
        calculateTickDeltas();

    // Check tickDelta
    if (((maximum_ - minimum_) / tickDelta_) > 1e4)
        return;

    // Plot tickmarks
    auto count = 0;
    auto delta = tickDelta_ / (nSubTicks_ + 1);
    auto value = tickStart_;
    std::vector<double> tickValues;
    while (value <= maximum_)
    {
        // Draw tick here, only if value >= minimum_
        if (value >= minimum_)
        {
            auto x = axisToGlobal(value);

            if (count % (nSubTicks_ + 1) == 0)
            {
                ticksEntity_.addVertices({{direction_ * x},{direction_ * x + tickDirection_ * tickScale_}});
                tickValues.push_back(value);

                count = 0;
            }
            else
            {
                subTicksEntity_.addVertices({{direction_ * x},{direction_ * x + tickDirection_ * tickScale_*0.5}});
            }
        }

        value += delta;
        ++count;
    }

    createTickLabelEntities(tickValues);
}

// Generate logarithmic ticks
void AxisEntity::generateLogarithmicTicks()
{
    // Check data range
    if (maximum_ < 0.0) {
        printf("Axis range is inappropriate for a log scale ({} < x < {}). Axis will not be drawn.\n",
               minimum_, maximum_);
        return;
    }

    // Grab logged min/max values for convenience, enforcing sensible minimum
    auto min = log10(minimum_ <= 0.0 ? 1.0e-10 : minimum_);

    // Plot tickmarks - Start at floored (ceiling'd) integer of logAxisMin (logAxisMax), and go from there.
    auto nMinorTicks = std::min(nSubTicks_, 9);
    auto count = 0;
    auto power = floor(min);
    auto value = pow(10, power);
    auto u = 0.0;
    while (value <= maximum_)
    {
        // If the current value is in range, plot a tick
        u = (inverted_ ? log10(maximum_ / value) : log10(value)) * axisScale_;
        if (log10(value) >= min)
        {
            // Tick mark
            if (count == 0)
                ticksEntity_.addVertices({{direction_ * axisScale_ * u},{direction_ * axisScale_ * u + tickDirection_ * tickScale_}});
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
}

// Map axis value to scaled global position
double AxisEntity::axisToGlobal(double axisValue) const {
    if (logarithmic_)
        return ((axisValue - minimum_) / (maximum_ - minimum_)) * axisScale_;
    else
        return ( (inverted_ ? (maximum_ - axisValue) : (axisValue - minimum_)) / (maximum_ - minimum_)) * axisScale_;
}

/*
 * Entities
 */

// Create / update tick labels at specified axis values
void AxisEntity::createTickLabelEntities(std::vector<double> values)
{
    // First, hide all existing label entities
    for (auto &&[entity, mesh, transform] : tickLabelMeshes_)
        entity->setEnabled(false);

    // Create any new text entities that we need
    while (tickLabelMeshes_.size() < values.size())
    {
        auto *entity = new Qt3DCore::QEntity(this);
        auto *mesh = new Qt3DExtras::QExtrudedTextMesh(this);
        auto *transform = new Qt3DCore::QTransform(entity);
        mesh->setFont(QFont("monospace"));
        mesh->setDepth(0.01);
        entity->addComponent(mesh);
        transform->setScale(fontScale_);
        entity->addComponent(transform);
        tickLabelMeshes_.emplace_back(entity, mesh, transform);
    }

    // Loop over new values and create / update entities as required
    // TODO Missing our zip operator here!
    auto n = 0;
    for (auto v : values)
    {
        auto &&[entity, mesh, transform] = tickLabelMeshes_[n];
        entity->setEnabled(true);
        mesh->setText(QString::number(v));
        transform->setTranslation({axisToGlobal(v), -(fontScale_ + tickScale_ + 0.05), 0.0});
        ++n;
    }
}

void AxisEntity::recreate()
{
    // Clear old primitives
    axisBarEntity_.clear();
    ticksEntity_.clear();
    subTicksEntity_.clear();

    // Plot basic axis line
    axisBarEntity_.addVertices({{0.0, 0.0, 0.0}, direction_*axisScale_});
    axisBarEntity_.setBasicIndices();
    axisBarEntity_.finalise();

    // Generate axis ticks
    if (logarithmic_)
        generateLogarithmicTicks();
    else
        generateLinearTicks();

    ticksEntity_.setBasicIndices();
    ticksEntity_.finalise();
    subTicksEntity_.setBasicIndices();
    subTicksEntity_.finalise();
}

// Add component to child entities
void AxisEntity::addComponentToChildren(Qt3DCore::QComponent *comp) {
    axisBarEntity_.addComponent(comp);
    ticksEntity_.addComponent(comp);
    subTicksEntity_.addComponent(comp);
    for (auto &&[entity, mesh, transform]: tickLabelMeshes_) {
        entity->addComponent(comp);
        auto a = mesh->maxPoint();
        auto b = mesh->minPoint();
        printf("Value  : TL = (%f, %f, %f) BR = (%f, %f, %f)\n", a.x(), a.y(), a.z(), b.x(), b.y(), b.z());
    }
}
