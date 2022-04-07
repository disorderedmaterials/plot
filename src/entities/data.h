#pragma once

#include "classes/cuboid.h"
#include "classes/metrics.h"
#include "entities/axis.h"
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <optional>

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
    // Extrema of displayed data in three-dimensional space
    std::optional<Cuboid> extrema_;

    public:
    // Return extreme limits for displayed data
    std::optional<Cuboid> extrema() const;

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

    public:
    // Set data entity material
    void setDataMaterial(Qt3DRender::QMaterial *material);

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
