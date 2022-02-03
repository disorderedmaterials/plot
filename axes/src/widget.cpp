#include "widget.h"
#include "axisentity.h"
#include <QResizeEvent>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QOrbitCameraController>
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

    // Construct a camera
    camera_ = new Qt3DRender::QCamera(rootEntity_.data());
    //    camera_->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setPosition(QVector3D(0, 0, 1.0f));
    camera_->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    auto *camController = new Qt3DExtras::QOrbitCameraController(rootEntity_.data());
    camController->setLinearSpeed(50.0f);
    camController->setLookSpeed(180.0f);
    camController->setCamera(camera_);

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

    if (localToSurfaceTransform_)
        localToSurfaceTransform_->setTranslation(metrics_.displayVolumeOrigin);

    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setAspectRatio(float(width()) / float(height()));
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
    // Create the top-level scene entity which will perform our global transform matrix
    sceneRootEntity_ = new Qt3DCore::QEntity(rootEntity_.data());
    localToSurfaceTransform_ = new Qt3DCore::QTransform(sceneRootEntity_);
    sceneRootEntity_->addComponent(localToSurfaceTransform_);

    /*
     * Axes Leaf
     */

    auto *axesEntity = new Qt3DCore::QEntity(sceneRootEntity_);
    displayVolumeTransform_ = new Qt3DCore::QTransform(axesEntity);
    displayVolumeTransform_->setTranslation(metrics_.displayVolumeOrigin);
    axesEntity->addComponent(displayVolumeTransform_);

    xAxis_ = new AxisEntity(AxisEntity::AxisType::Horizontal, axesEntity);
    yAxis_ = new AxisEntity(AxisEntity::AxisType::Vertical, axesEntity);
    zAxis_ = new AxisEntity(AxisEntity::AxisType::Depth, axesEntity);
    zAxis_->setEnabled(false);

    /*
     * Data Space Leaf
     */

    auto *dataEntity = new Qt3DCore::QEntity(sceneRootEntity_);
    displayVolumeTransform_ = new Qt3DCore::QTransform(dataEntity);
    dataEntity->addComponent(displayVolumeTransform_);

    // Test Scalable Central View Volume
    auto *boxMaterial = new Qt3DExtras::QDiffuseSpecularMaterial(dataEntity);
    boxMaterial->setAmbient(Qt::red);
    auto *boxEntity = new LineEntity(dataEntity, Qt3DRender::QGeometryRenderer::LineLoop);
    boxEntity->addVertices({{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}});
    boxEntity->setBasicIndices();
    boxEntity->finalise();
    boxEntity->addComponent(boxMaterial);
}

// Return x axis entity
const AxisEntity *MildredWidget::xAxis() const { return xAxis_; }

// Return y axis entity
const AxisEntity *MildredWidget::yAxis() const { return yAxis_; }

// Return z axis entity
const AxisEntity *MildredWidget::zAxis() const { return zAxis_; }

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
