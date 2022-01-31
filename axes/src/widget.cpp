#include "widget.h"
#include "axisentity.h"
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCuboidMesh>
#include <QResizeEvent>

#include <Qt3DExtras/QForwardRenderer>

MildredWidget::MildredWidget(QWidget *parent) : QWidget(parent)
{
    /*
     * In order to get a suitable surface to draw on we must first create a full Qt3DWindow and then capture it in
     * a container widget. Is there a Qt3DWidget which can simplify this?
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
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera_);

    // Create the framegraph
    frameGraph_.create(renderSettings_, viewWindow_, camera_);

    // Set up basic scenegraph
    createSceneGraph();

    viewWindow_->setRootEntity(rootEntity_.data());
}

/*
 * QWidget
 */

void MildredWidget::resizeEvent(QResizeEvent *event)
{
    surfaceXScaleFactor_ = float(width())/120.0f;
    surfaceYScaleFactor_ = float(height())/120.0f;
    if (localToSurfaceTransform_)
    {
        localToSurfaceTransform_->setScale3D({surfaceXScaleFactor_, surfaceYScaleFactor_, 1.0});
//        localToSurfaceTransform_->setTranslation({surfaceXScaleFactor_ * 10.0f, surfaceYScaleFactor_* 10.0f, 0.0f});
//        localToSurfaceTransform_->setRotationZ(45.0);
    }
//    if (viewVolumeTransform_) viewVolumeTransform_->setScale3D({100.0,100.0, 1.0});
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setAspectRatio(float(width()) / float(height()));
    viewContainer_->resize(this->size());
}

/*
 * SceneGraph
 */

void MildredWidget::createSceneGraph()
{
    // Transform from device (width x height) to local view
    localToSurfaceTransform_ = new Qt3DCore::QTransform(rootEntity_.data());
    rootEntity_->addComponent(localToSurfaceTransform_);

    /*
     * Axes Leaf
     */

    auto *axesEntity = new Qt3DCore::QEntity(rootEntity_.data());
    auto *axesTransform = new Qt3DCore::QTransform(axesEntity);
    axesTransform->setTranslation({10.0, 10.0, 0.0});
    axesEntity->addComponent(axesTransform);

    // Components
    auto *axesMaterial = new Qt3DExtras::QPhongMaterial(axesEntity);
    axesMaterial->setAmbient(Qt::black);

    // Axes
    auto *xAxis = new AxisEntity(axesEntity);
    xAxis->recreate(surfaceXScaleFactor_, surfaceYScaleFactor_);
//    xAxis->setAxisScale(100.0);
//    xAxis->setTickScale(2.0);
    xAxis->addComponentToChildren(axesMaterial);

    /*
     * Data Space Leaf
     */
    auto *dataEntity = new Qt3DCore::QEntity(rootEntity_.data());
    viewVolumeTransform_ = new Qt3DCore::QTransform(dataEntity);
    dataEntity->addComponent(viewVolumeTransform_);

    // Transform

    // Test Scalable Central View Volume
    auto *boxMaterial = new Qt3DExtras::QPhongMaterial(dataEntity);
    boxMaterial->setAmbient(Qt::red);
    auto *boxEntity = new LineEntity(dataEntity, Qt3DRender::QGeometryRenderer::LineLoop);
    boxEntity->addVertices({{0.0,0.0,0.0},{1.0,0.0,0.0},{1.0,1.0,0.0},{0.0,1.0,0.0}});
    boxEntity->setBasicIndices();
    boxEntity->finalise();
    boxEntity->addComponent(boxMaterial);
}
