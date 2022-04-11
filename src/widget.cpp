#include "widget.h"
#include "entities/axis.h"
#include "materials/material.h"
#include <QResizeEvent>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

//! Constructs a Mildred widget which is a child of \param parent.
MildredWidget::MildredWidget(QWidget *parent) : QWidget(parent)
{
    // Initialise our resources
    Q_INIT_RESOURCE(shaders);

    /*
     * In order to get a suitable surface to draw on we must first create a full Qt3DWindow and then capture it in
     * a container widget.
     */
    viewWindow_ = new Qt3DExtras::Qt3DWindow();

    // Create a container for the Qt3DWindow
    viewContainer_ = createWindowContainer(viewWindow_, this);

    // Create our root entity
    rootEntity_ = Qt3DCore::QEntityPtr(new Qt3DCore::QEntity);

    // Create a QRenderSettings and add it as a component to the root entity
    renderSettings_ = viewWindow_->renderSettings();

    // Create parameters
    sceneDataAxesParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxes"), QMatrix4x4());
    sceneDataAxesExtentsParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesExtents"), QVector3D());
    sceneDataAxesOriginParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesOrigin"), QVector3D());
    sceneDataTransformInverseParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataTransformInverse"), QMatrix4x4());
    viewportSizeParameter_ = new Qt3DRender::QParameter(QStringLiteral("viewportSize"), QVector2D());

    // Add a mouse handler and connect it up
    auto *mouseHandler = new Qt3DInput::QMouseHandler(rootEntity_.data());
    auto *mouseDevice = new Qt3DInput::QMouseDevice(rootEntity_.data());
    mouseHandler->setSourceDevice(mouseDevice);
    rootEntity_->addComponent(mouseHandler);
    connect(mouseHandler, SIGNAL(positionChanged(Qt3DInput::QMouseEvent *)), this,
            SLOT(mousePositionChanged(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(pressed(Qt3DInput::QMouseEvent *)), this, SLOT(mouseButtonPressed(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent *)), this,
            SLOT(mouseButtonReleased(Qt3DInput::QMouseEvent *)));

    // Construct a camera
    camera_ = new Qt3DRender::QCamera(rootEntity_.data());
    //    camera_->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setPosition(QVector3D(0, 0, 1.0f));
    camera_->setViewCenter(QVector3D(0, 0, -10.0));

    // Create the framegraph
    frameGraph_.create(renderSettings_, viewWindow_, camera_);

    // Set up basic scenegraph
    createSceneGraph();

    // Set the main root entity
    viewWindow_->setRootEntity(rootEntity_.data());

    // Connect the metrics object and update
    connect(&metrics_, SIGNAL(metricsChanged()), this, SLOT(updateTransforms()));
}

/*
 * QWidget
 */

//! Handle QWidget resize events
/*!
 * Resizing the widget demands that the metrics information held in @class MildredMetrics is updated, ensuring the whole of the
 * available drawing surface is used for visualisation.
 */
void MildredWidget::resizeEvent(QResizeEvent *event)
{
    updateMetrics();

    // Move the scene root position to be the centre of the XY plane and a suitable distance away
    sceneRootTransform_->setTranslation(QVector3D(width() / 2.0, height() / 2.0, -width()));

    // Reset projection for new viewport
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, width() * 2.0f);
    camera_->setAspectRatio(float(width()) / float(height()));

    // Debug objects
    sceneBoundingCuboidTransform_->setScale3D(QVector3D(width(), height(), width()));

    // Lastly, resize our view container
    viewContainer_->resize(this->size());
}

/*
 * Metrics
 */

//! Update metrics for current surface size
/*!
 *  Updates the internal @class MildredMetrics object.
 */
void MildredWidget::updateMetrics() { metrics_.update(width(), height(), xAxis_, yAxis_); }

/*
 * Appearance
 */

//! Return whether the view is flat
bool MildredWidget::isFlatView() const { return flatView_; }

//! Set whether view is flat
/*!
 * This controls whether the current view is set to fixed, flat (2D) drawing (@param flat = true) with only the x and y axes
 * shown, or the view is full 3D in which case all axes (including the depth z axis) are visible.
 *
 * Changing the view type necessarily enforces a recalculation of the metrics object.
 */
void MildredWidget::setFlatView(bool flat)
{
    if (flatView_ == flat)
        return;

    flatView_ = flat;

    // Set z-axis visibility
    zAxis_->setEnabled(!flatView_);

    // Reset view and update
    resetView();
    updateMetrics();
}

/*
 * SceneGraph
 */

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
    auto *cuboidMaterial = new Qt3DExtras::QDiffuseSpecularMaterial(sceneBoundingCuboidEntity_);
    cuboidMaterial->setAmbient(QColor(255, 0, 0, 100));
    cuboidMaterial->setAlphaBlendingEnabled(true);
    sceneBoundingCuboidEntity_->addComponent(cuboidMaterial);

    /*
     * Axes Leaf
     */

    auto *axesEntity = new Qt3DCore::QEntity(sceneObjectsEntity_);
    xAxis_ = new AxisEntity(AxisEntity::AxisType::Horizontal, metrics_, axesEntity);
    connect(xAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    yAxis_ = new AxisEntity(AxisEntity::AxisType::Vertical, metrics_, axesEntity);
    connect(yAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    zAxis_ = new AxisEntity(AxisEntity::AxisType::Depth, metrics_, axesEntity);
    connect(zAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    zAxis_->setEnabled(false);

    /*
     * Data Space Leaf
     */

    dataRootEntity_ = new Qt3DCore::QEntity(sceneObjectsEntity_);
    dataEntityParent_ = new Qt3DCore::QEntity(dataRootEntity_);
    dataOriginTransform_ = new Qt3DCore::QTransform(dataEntityParent_);
    dataEntityParent_->addComponent(dataOriginTransform_);

    // TEST
    auto *sphereEntity_ = new Qt3DCore::QEntity(dataEntityParent_);
    auto *sphereMesh = new Qt3DExtras::QSphereMesh(sphereEntity_);
    sphereEntity_->addComponent(sphereMesh);
    auto *sphereMaterial = new RenderableMaterial(sphereEntity_);
    sphereMaterial->addParameter(sceneDataAxesParameter_);
    sphereMaterial->addParameter(sceneDataAxesExtentsParameter_);
    sphereMaterial->addParameter(sceneDataAxesOriginParameter_);
    sphereMaterial->addParameter(sceneDataTransformInverseParameter_);
    sphereMaterial->addParameter(viewportSizeParameter_);
    sphereEntity_->addComponent(sphereMaterial);
    auto *sphereTransform = new Qt3DCore::QTransform();
    sphereTransform->setScale(50.0);
    sphereTransform->setTranslation(QVector3D(530.0, 50, 0));
    sphereEntity_->addComponent(sphereTransform);
}

//! Return x axis entity
const AxisEntity *MildredWidget::xAxis() const { return xAxis_; }

//! Return y axis entity
const AxisEntity *MildredWidget::yAxis() const { return yAxis_; }

//! Return z axis entity
const AxisEntity *MildredWidget::zAxis() const { return zAxis_; }

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

/*
 * Mouse Handling
 */

//! React to mouse movement
/*!
 * React to mouse move events signalled by a Qt3DMouseHandler attached to the Qt3D surface, potentially affecting changes to the
 * axes ranges or view volume, depending on the current view type and options.
 *
 * Active mouse buttons modify the effect of the event, and which also depends on the current view mode and depressed modifier
 * keys.
 *
 * | Button | View Type | Modifier | Action |
 * | :----: | :-------: | :------: | ------ |
 * | Left   | Flat / 2D | None     | Update current coordinate under the mouse. |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     | None   |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | Middle | Flat / 2D | None     | Translate view in the XY plane, modifying the corresponding ranges of the x and y axes |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     |        |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | Right  | Flat / 2D | None     | None   |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     | Rotate view volume around its centroid. |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 */
void MildredWidget::mousePositionChanged(Qt3DInput::QMouseEvent *event)
{
    // Check previous position
    if (lastMousePosition_.isNull())
    {
        lastMousePosition_ = QPoint(event->x(), event->y());
        return;
    }

    // Right button - Rotate scene volume (3D)
    if (event->buttons() & Qt3DInput::QMouseEvent::RightButton)
    {
        // Rotations only allowed for 3D view
        if (!flatView_)
        {
            viewRotationMatrix_ *=
                QQuaternion::fromEulerAngles(event->y() - lastMousePosition_.y(), event->x() - lastMousePosition_.x(), 0.0);
            sceneRootTransform_->setRotation(viewRotationMatrix_);
        }
    }

    // Middle button - translate axis ranges (2D)
    if (event->buttons() & Qt3DInput::QMouseEvent::MiddleButton)
    {
        if (flatView_)
        {
            xAxis_->shiftLimits(-xAxis_->range() *
                                (double(event->x() - lastMousePosition_.x()) / metrics_.displayVolumeExtent().x()));
            yAxis_->shiftLimits(yAxis_->range() *
                                (double(event->y() - lastMousePosition_.y()) / metrics_.displayVolumeExtent().y()));
            updateTransforms();
        }
    }

    updateShaderParameters();

    lastMousePosition_ = QPoint(event->x(), event->y());
}

void MildredWidget::mouseButtonPressed(Qt3DInput::QMouseEvent *event) {}

void MildredWidget::mouseButtonReleased(Qt3DInput::QMouseEvent *event) {}

/*
 * Display Data
 */

// Add new 1-dimensional data entity for supplied data
Data1DEntity *MildredWidget::addData1D(std::string_view tag)
{
    // Check for existing tag
    auto it = std::find_if(dataEntities_.begin(), dataEntities_.end(), [tag](const auto &d) { return tag == d.first; });
    if (it != dataEntities_.end())
    {
        printf("Data with tag '%s' already exists, so can't add it again. Returning the existing data instead.\n",
               it->first.c_str());
        throw(std::runtime_error("Duplicate DataEntity tag created.\n"));
    }

    // Create a new entity
    auto *entity = new Data1DEntity(xAxis_, yAxis_, dataEntityParent_);
    connect(&metrics_, SIGNAL(metricsChanged()), entity, SLOT(updateRenderables()));
    dataEntities_.emplace_back(tag, entity);

    return entity;
}

/*
 * Slots
 */

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
