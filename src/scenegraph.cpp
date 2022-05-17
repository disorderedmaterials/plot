#include "entities/axis.h"
#include "material.h"
#include "widget.h"
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DRender/QPointLight>

using namespace Mildred;

//! Creates the scene graph for the display.
/*!
 * The Qt3D scene graph is created with the following layout.
 *
 *          [RootEntity]               Top-level root entity
 *               |
 *         sceneRootEntity_
 *          |           |
 * sceneRootTransform_  |              Translates to distant rotation point and applies 3D view rotation
 *                      |
 *              sceneObjectsEntity_    Contains all viewable objects for plotting
 *               |         |    |
 * sceneObjectsTransform_  |    |      Places scene objects so that global 0,0,0 is lower left corner to the viewer
 *                         |    |
 *                 axesEntity   |      Contains the individual AxesEntities
 *                     |        |
 *           xAxis_,yAxis_...   |      Individual axis entities
 *                              |
 *                      dataEntity     Parent entity for all displayed data series
 *                       |      |
 *      dataOriginTransform_    *      Sets the coordinate system so that 0,0,0 is at the axes origin
 */
void MildredWidget::createSceneGraph()
{
    auto *lightEntity = new Qt3DCore::QEntity(rootEntity_.data());

    auto *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);

    auto *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(lightPosition_);
    lightEntity->addComponent(lightTransform);

    sceneRootEntity_ = new Qt3DCore::QEntity(rootEntity_.data());
    sceneRootTransform_ = new Qt3DCore::QTransform(sceneRootEntity_);
    sceneRootEntity_->addComponent(sceneRootTransform_);

    auto *sceneObjectsEntity_ = new Qt3DCore::QEntity(sceneRootEntity_);
    sceneObjectsTransform_ = new Qt3DCore::QTransform(sceneObjectsEntity_);
    sceneObjectsEntity_->addComponent(sceneObjectsTransform_);

    // Debug
    sceneBoundingCuboidEntity_ = new Qt3DCore::QEntity(sceneRootEntity_);
    sceneBoundingCuboidEntity_->setEnabled(false);
    auto *cuboidMesh = new Qt3DExtras::QCuboidMesh(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->addComponent(cuboidMesh);
    sceneBoundingCuboidTransform_ = new Qt3DCore::QTransform(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->addComponent(sceneBoundingCuboidTransform_);
    auto *cuboidMaterial =
        createMaterial(sceneBoundingCuboidEntity_, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::LineTesselator, RenderableMaterial::FragmentShaderType::Phong);
    cuboidMaterial->setAmbient(QColor(255, 0, 0, 255));
    sceneBoundingCuboidEntity_->addComponent(cuboidMaterial);

    /*
     * Axes Leaf
     */

    auto *axesEntity = new Qt3DCore::QEntity(sceneObjectsEntity_);

    auto *xAxisBarMaterial = createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                                            RenderableMaterial::GeometryShaderType::LineTesselator,
                                            RenderableMaterial::FragmentShaderType::Monochrome);
    xAxisBarMaterial->setAmbient(QColor(0, 0, 0, 255));
    auto *xAxisLabelMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Monochrome);
    xAxisLabelMaterial->setAmbient(QColor(0, 0, 0, 255));
    xAxis_ = new AxisEntity(axesEntity, AxisEntity::AxisType::Horizontal, metrics_, xAxisBarMaterial, xAxisLabelMaterial);
    xAxis_->setTitleText("X");
    connect(xAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    connect(xAxis_, SIGNAL(rangeChanged()), this, SLOT(updateMetrics()));

    auto *yAxisBarMaterial = createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                                            RenderableMaterial::GeometryShaderType::LineTesselator,
                                            RenderableMaterial::FragmentShaderType::Monochrome);
    yAxisBarMaterial->setAmbient(QColor(0, 0, 0, 255));
    auto *yAxisLabelMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Monochrome);
    yAxisLabelMaterial->setAmbient(QColor(0, 0, 0, 255));
    yAxis_ = new AxisEntity(axesEntity, AxisEntity::AxisType::Vertical, metrics_, yAxisBarMaterial, yAxisLabelMaterial);
    yAxis_->setTitleText("Y");
    connect(yAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    connect(yAxis_, SIGNAL(rangeChanged()), this, SLOT(updateMetrics()));

    auto *zAxisBarMaterial = createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                                            RenderableMaterial::GeometryShaderType::LineTesselator,
                                            RenderableMaterial::FragmentShaderType::Monochrome);
    zAxisBarMaterial->setAmbient(QColor(0, 0, 0, 255));
    auto *zAxisLabelMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Monochrome);
    zAxisLabelMaterial->setAmbient(QColor(0, 0, 0, 255));
    zAxis_ = new AxisEntity(axesEntity, AxisEntity::AxisType::Depth, metrics_, zAxisBarMaterial, zAxisLabelMaterial);
    zAxis_->setTitleText("Z");
    connect(zAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    connect(zAxis_, SIGNAL(rangeChanged()), this, SLOT(updateMetrics()));
    zAxis_->setEnabled(false);

    /*
     * Data Space Leaf
     */

    dataRootEntity_ = new Qt3DCore::QEntity(sceneObjectsEntity_);
    dataEntityParent_ = new Qt3DCore::QEntity(dataRootEntity_);
    dataOriginTransform_ = new Qt3DCore::QTransform(dataEntityParent_);
    dataEntityParent_->addComponent(dataOriginTransform_);
}

//! Convert widget position to 2D (flat) coordinates
QPointF MildredWidget::toAxes2D(QPoint pos) const
{
    return {xAxis_->fromScaled((pos.x() - width() / 2.0) - sceneObjectsTransform_->translation().x()),
            yAxis_->fromScaled((pos.y() - height() / 2.0) - sceneObjectsTransform_->translation().y())};
}

//! Return screen coordinates at centre of 2D view
QPoint MildredWidget::screen2DCentre() const
{
    return {int(width() / 2 + sceneObjectsTransform_->translation().x() + metrics_.displayVolumeExtent().x() / 2.0),
            int(height() / 2 + sceneObjectsTransform_->translation().y() + metrics_.displayVolumeExtent().y() / 2.0)};
}

//! Return x axis entity
AxisEntity *MildredWidget::xAxis() { return xAxis_; }

//! Return y axis entity
AxisEntity *MildredWidget::yAxis() { return yAxis_; }

//! Return z axis entity
AxisEntity *MildredWidget::zAxis() { return zAxis_; }

//! Update transforms from metrics
/*!
 * The metrics defined in the @class MildredMetrics object contains the offsets in 3D space for the axes origin and general
 * graphing volume. This slot is connected internally to signals in the @class MildredMetrics object to ensure correct update of
 * the view when the metrics have been recalculated.
 */
void MildredWidget::updateTransforms()
{
    if (sceneObjectsTransform_)
        sceneObjectsTransform_->setTranslation(metrics_.displayVolumeOrigin() -
                                               QVector3D(width() / 2.0, height() / 2.0, -width() / 2.0));
    if (dataOriginTransform_)
        dataOriginTransform_->setTranslation(
            -(xAxis_->toScaled(xAxis_->minimum()) + yAxis_->toScaled(yAxis_->minimum()) + zAxis_->toScaled(zAxis_->minimum())));
}

//! Update shader parameters
/*!
 * Update shader parameters from current view information.
 */
void MildredWidget::updateShaderParameters()
{
    sceneDataAxesParameter_->setValue(QMatrix4x4(xAxis_->direction().x(), xAxis_->direction().y(), xAxis_->direction().z(), 0.0,
                                                 yAxis_->direction().x(), yAxis_->direction().y(), yAxis_->direction().z(), 0.0,
                                                 zAxis_->direction().x(), zAxis_->direction().y(), zAxis_->direction().z(), 0.0,
                                                 0.0, 0.0, 0.0, 1.0));
    sceneDataAxesExtentsParameter_->setValue(metrics_.displayVolumeExtent());
    sceneDataAxesOriginParameter_->setValue(xAxis_->toScaled(xAxis_->minimum()) + yAxis_->toScaled(yAxis_->minimum()) +
                                            zAxis_->toScaled(zAxis_->minimum()));
    sceneDataTransformInverseParameter_->setValue(
        (sceneRootTransform_->matrix() * sceneObjectsTransform_->matrix() * dataOriginTransform_->matrix()).inverted());
    viewportSizeParameter_->setValue(QVector2D(width(), height()));
}

//! Reset view
/*!
 * Resets the main scene view rotation to the identity matrix
 */
void MildredWidget::resetView()
{
    assert(sceneRootTransform_);

    viewRotationMatrix_ = QQuaternion();
    sceneRootTransform_->setRotation(viewRotationMatrix_);

    updateShaderParameters();
}

//! Show all data in view
/*!
 * Reset axis limits to show all visible data
 */
void MildredWidget::showAllData()
{
    Cuboid extrema, logarithmicExtrema;
    for (auto &[name, entity] : dataEntities_)
    {
        // Don't adjust extents for hidden data
        if (!entity->isEnabled())
            continue;

        extrema.expand(entity->extrema());
        logarithmicExtrema.expand(entity->logarithmicExtrema());
    }

    // Overwrite linear with log extents for any log axes we have
    if (xAxis_->isLogarithmic() && logarithmicExtrema.validXExtent())
        extrema.setXExtent(pow(10.0, logarithmicExtrema.lowerLeftBack().x()),
                           pow(10.0, logarithmicExtrema.upperRightFront().x()));
    if (yAxis_->isLogarithmic() && logarithmicExtrema.validYExtent())
        extrema.setYExtent(pow(10.0, logarithmicExtrema.lowerLeftBack().y()),
                           pow(10.0, logarithmicExtrema.upperRightFront().y()));
    if (zAxis_->isLogarithmic() && logarithmicExtrema.validZExtent())
        extrema.setZExtent(pow(10.0, logarithmicExtrema.lowerLeftBack().z()),
                           pow(10.0, logarithmicExtrema.upperRightFront().z()));

    // Don't allow zero range on any axis
    // TODO Checking on absolute value is going to be unsuited to all eventualities
    if (!extrema.validXExtent() || extrema.xExtent() < 1.0e-8)
        extrema.expandX(1.0);
    if (!extrema.validYExtent() || extrema.yExtent() < 1.0e-8)
        extrema.expandY(1.0);
    if (!extrema.validZExtent() || extrema.zExtent() < 1.0e-8)
        extrema.expandZ(1.0);

    xAxis_->setLimits(extrema.lowerLeftBack().x(), extrema.upperRightFront().x());
    yAxis_->setLimits(extrema.lowerLeftBack().y(), extrema.upperRightFront().y());
    zAxis_->setLimits(extrema.lowerLeftBack().z(), extrema.upperRightFront().z());

    updateMetrics();
}

//! Change the text of the x-axis title.
/*!
 * Set the displayed text of the x-axis title to @param title. Metrics are updated after the change.
 */
void MildredWidget::setXAxisTitle(const QString &title)
{
    assert(xAxis_);
    xAxis_->setTitleText(title);
    updateMetrics();
}

//! Change the text of the y-axis title.
/*!
 * Set the displayed text of the y-axis title to @param title. Metrics are updated after the change.
 */
void MildredWidget::setYAxisTitle(const QString &title)
{
    assert(yAxis_);
    yAxis_->setTitleText(title);
    updateMetrics();
}

//! Change the text of the z-axis title.
/*!
 * Set the displayed text of the z-axis title to @param title. Metrics are updated after the change.
 */
void MildredWidget::setZAxisTitle(const QString &title)
{
    assert(zAxis_);
    zAxis_->setTitleText(title);
    updateMetrics();
}

//! Change the visibility of the scene debug cuboid.
/*!
 * Sets whether the scene debug cuboid is enabled (visible), and which illustrates the current view volume extent.
 */
void MildredWidget::setSceneCuboidEnabled(bool enabled)
{
    assert(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->setEnabled(enabled);
}
