#include "widget.h"
#include "axisentity.h"
#include <QResizeEvent>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

#include <Qt3DExtras/QForwardRenderer>

MildredWidget::MildredWidget(QWidget *parent) : QWidget(parent)
{
    // Set up a suitable monospace axisTickLabelFont
    metrics_.axisTickLabelFont = QFont("Tahoma", 10.0);
    metrics_.axisTitleFont = QFont("Tahoma", 11.0);

    /*
     * In order to get a suitable surface to draw on we must first create a full Qt3DWindow and then capture it in
     * a container widget. Is there not a Qt3DWidget which can simplify this?
     */
    viewWindow_ = new Qt3DExtras::Qt3DWindow();

    // create a container for Qt3DWindow
    viewContainer_ = createWindowContainer(viewWindow_, this);

    // Create our root entity
    rootEntity_ = Qt3DCore::QEntityPtr(new Qt3DCore::QEntity);

    // Create a QRenderSettings and add it as a component to the root entity
    renderSettings_ = new Qt3DRender::QRenderSettings(rootEntity_.data());
    rootEntity_->addComponent(renderSettings_);

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

    viewWindow_->setRootEntity(rootEntity_.data());

    updateMetrics();
}

/*
 * QWidget
 */

void MildredWidget::resizeEvent(QResizeEvent *event)
{
    updateMetrics(width(), height());

    // Move the scene root position to be the centre of the XY plane and a suitable distance away
    sceneRootTransform_->setTranslation(QVector3D(width() / 2.0, height() / 2.0, -width()));

    // Set the local transform so global origin corresponds to the visible screen origin
    if (sceneObjectsTransform_)
        sceneObjectsTransform_->setTranslation(metrics_.displayVolumeOrigin -
                                               QVector3D(width() / 2.0, height() / 2.0, -width() / 2.0));

    dataOriginTransform_->setTranslation(metrics_.displayVolumeOrigin);

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

// Update metrics for specified surface size
void MildredWidget::updateMetrics(int width, int height)
{
    QFontMetrics fontMetrics(metrics_.axisTickLabelFont);

    // Initialise the display volume origin to zero (pixels) and the extent to the max surface size
    metrics_.displayVolumeOrigin = QVector3D(0, 0, 0);
    metrics_.displayVolumeExtent = QVector3D(float(width), float(height), float(width + height) / 2.0f);

    // Apply margin around extreme edge of surface
    metrics_.displayVolumeOrigin += QVector3D(metrics_.nMarginPixels, metrics_.nMarginPixels, 0.0);
    metrics_.displayVolumeExtent -= QVector3D(2.0f * metrics_.nMarginPixels, 2.0f * metrics_.nMarginPixels, 0.0);

    // Reduce display volume to accommodate axes
    if (xAxis_ && xAxis_->isEnabled())
    {
        auto xRect = xAxis_->boundingRect(metrics_);
        metrics_.displayVolumeOrigin += QVector3D(0.0, xRect.yExtent(), 0.0);
        metrics_.displayVolumeExtent -= QVector3D(0.0, xRect.yExtent(), 0.0);
    }
    if (yAxis_ && yAxis_->isEnabled())
    {
        auto yRect = yAxis_->boundingRect(metrics_);
        metrics_.displayVolumeOrigin += QVector3D(yRect.xExtent(), 0.0, 0.0);
        metrics_.displayVolumeExtent -= QVector3D(yRect.xExtent(), 0.0, 0.0);
    }

    // Recreate visible axes
    if (xAxis_ && xAxis_->isEnabled())
        xAxis_->recreate(metrics_);
    if (yAxis_ && yAxis_->isEnabled())
        yAxis_->recreate(metrics_);
    if (zAxis_ && zAxis_->isEnabled())
        zAxis_->recreate(metrics_);
}

// Update metrics for current surface size
void MildredWidget::updateMetrics() { updateMetrics(width(), height()); }

/*
 * SceneGraph
 */

void MildredWidget::createSceneGraph()
{
    /*
     *    Scene Graph Layout
     *    ------------------
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

    xAxis_ = new AxisEntity(AxisEntity::AxisType::Horizontal, axesEntity);
    yAxis_ = new AxisEntity(AxisEntity::AxisType::Vertical, axesEntity);
    zAxis_ = new AxisEntity(AxisEntity::AxisType::Depth, axesEntity);
    zAxis_->setEnabled(false);

    /*
     * Data Space Leaf
     */

    auto *dataEntity = new Qt3DCore::QEntity(sceneObjectsEntity_);
    dataOriginTransform_ = new Qt3DCore::QTransform(dataEntity);
    dataEntity->addComponent(dataOriginTransform_);
}

// Return x axis entity
const AxisEntity *MildredWidget::xAxis() const { return xAxis_; }

// Return y axis entity
const AxisEntity *MildredWidget::yAxis() const { return yAxis_; }

// Return z axis entity
const AxisEntity *MildredWidget::zAxis() const { return zAxis_; }

/*
 * Mouse Handling
 */

void MildredWidget::mousePositionChanged(Qt3DInput::QMouseEvent *event)
{
    // Check previous position
    if (lastMousePosition_.isNull())
    {
        lastMousePosition_ = QPoint(event->x(), event->y());
        return;
    }

    // Rotate scene volume (right mouse button pressed)
    if (event->buttons() & Qt3DInput::QMouseEvent::RightButton)
    {
        auto q = QQuaternion::fromEulerAngles(event->y() - lastMousePosition_.y(), event->x() - lastMousePosition_.x(), 0.0) *
                 sceneRootTransform_->rotation();
        sceneRootTransform_->setRotation(q);
    }

    lastMousePosition_ = QPoint(event->x(), event->y());
}

void MildredWidget::mouseButtonPressed(Qt3DInput::QMouseEvent *event) {}

void MildredWidget::mouseButtonReleased(Qt3DInput::QMouseEvent *event) {}

/*
 * Slots
 */

void MildredWidget::setXAxisTitle(const QString &title)
{
    assert(xAxis_);
    xAxis_->setTitleText(title);
    updateMetrics();
}

void MildredWidget::setYAxisTitle(const QString &title)
{
    assert(yAxis_);
    yAxis_->setTitleText(title);
    updateMetrics();
}

void MildredWidget::setZAxisTitle(const QString &title)
{
    assert(zAxis_);
    zAxis_->setTitleText(title);
    updateMetrics();
}

void MildredWidget::setSceneCuboidEnabled(bool enabled)
{
    assert(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->setEnabled(enabled);
}
