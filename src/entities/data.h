#pragma once

#include "classes/colourdefinition.h"
#include "classes/cuboid.h"
#include "classes/metrics.h"
#include "entities/axis.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <optional>

namespace Mildred
{
//! DataEntity represents a renderable dataset.
/*!
 * DataEntity provides the base class to all data renderables.
 *
 * @see Data1DEntity
 * @see Data2DEntity
 * @see Data3DEntity
 */
class DataEntity : public Qt3DCore::QEntity
{
    Q_OBJECT

    public:
    explicit DataEntity(Qt3DCore::QNode *parent = nullptr);
    ~DataEntity() override = default;

    /*
     * Data
     */
    protected:
    // Display name for entity
    std::string displayName_;
    // Extrema of data points in three-dimensional space
    Cuboid extrema_;
    // Extrema of data points in three-dimensional logarithmic space
    Cuboid logarithmicExtrema_;

    public:
    // Return extrema of data points in three-dimensional space
    Cuboid extrema() const;
    // Return extrema of data points in three-dimensional logarithmic space
    Cuboid logarithmicExtrema() const;

    protected:
    void updateExtrema(std::optional<double> x, std::optional<double> y, std::optional<double> z);

    /*
     * Entities
     */
    protected:
    // Entity providing data display
    Qt3DCore::QEntity *dataEntity_{nullptr};
    // Entity providing error display
    Qt3DCore::QEntity *errorEntity_{nullptr};
    // Entity providing symbol display
    Qt3DCore::QEntity *symbolEntity_{nullptr};

    /*
     * Components
     */
    private:
    // Transform in 3D space, used internally for special positioning
    Qt3DCore::QTransform *positionalTransform_{nullptr};
    // Material for data entity
    Qt3DRender::QMaterial *dataEntityMaterial_{nullptr};
    // Material for error entity
    Qt3DRender::QMaterial *errorEntityMaterial_{nullptr};
    // Material for symbol entity
    Qt3DRender::QMaterial *symbolEntityMaterial_{nullptr};

    protected:
    // Local colour definition for entity
    ColourDefinition colour_;
    // Colour definition override
    std::optional<ColourDefinition> colourOverride_;

    protected:
    // Return colour definition to use
    ColourDefinition colourDefinition() const;

    public:
    // Return local colour definition for entity
    ColourDefinition &colour();
    const ColourDefinition &colour() const;
    // Set override colour definition (e.g. from group)
    void setColourOverride(const ColourDefinition &colour);
    // Remove colour definition override
    void removeColourOverride();
    // Set data entity material
    void setDataMaterial(Qt3DRender::QMaterial *material);
    // Get the data entity material
    Qt3DRender::QMaterial *dataMaterial();
    // Set error entity material
    void setErrorMaterial(Qt3DRender::QMaterial *material);
    // Get the error entity material
    Qt3DRender::QMaterial *errorMaterial();
    // Set symbol entity material
    void setSymbolMaterial(Qt3DRender::QMaterial *material);
    // Get the symbol entity material
    Qt3DRender::QMaterial *symbolMaterial();

    /*
     * Rendering
     */
    protected:
    // Create renderables from current data
    virtual void create() = 0;

    public slots:
    // Recreate renderables following metric / axis change
    void updateRenderables();
};
} // namespace Mildred