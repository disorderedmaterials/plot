#include <QGuiApplication>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <QPropertyAnimation>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

#include "axisentity.h"

void createFrameGraph(Qt3DRender::QRenderSettings *renderSettings, QObject *surface, Qt3DRender::QCamera *camera)
{
    /*
     * Set up a simple forward renderer framgraph. Nodes are contructed as a single-leaf stack:
     *
     *        [QRenderSettings]         Parent of first framegraph node
     *               |
     *        QSurfaceSelector          Sets the target surface for the renderer
     *               |
     *           QViewport              Defines the viewport on the target surface
     *               |
     *         QClearBuffers            Sets the targets to clear and the default attributes to clear to
     *               |
     *        QCameraSelector           Selects an existing camera to view the scenegraph
     */

    // Set our rendering surface
    auto *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(renderSettings);
    surfaceSelector->setSurface(surface);

    // Define a viewport to cover the entire surface
    auto *viewport = new Qt3DRender::QViewport(surfaceSelector);
    viewport->setNormalizedRect({0.0, 0.0, 1.0, 1.0});

    // Clear to background colour
    auto *clearBuffers = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor(QColor(255,255,255,255));

    // Set the camera to the one we passed in
    auto *cameraSelector = new Qt3DRender::QCameraSelector(clearBuffers);
    cameraSelector->setCamera(camera);

    // Finally, set the active framegraph of the QRenderSettings to the top node of our framegrsph
    renderSettings->setActiveFrameGraph(surfaceSelector);
}

void createSceneGraph(Qt3DCore::QEntity *root)
{
    // Components
    auto *axesMaterial = new Qt3DExtras::QPhongMaterial(root);
    axesMaterial->setAmbient(Qt::black);

    // Axes
    auto *xAxis = new AxisEntity(root);
    xAxis->addComponentToChildren(axesMaterial);
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    // Create the root entity
    Qt3DCore::QEntityPtr rootEntity(new Qt3DCore::QEntity);

    // Create a QRenderSettings and add it as a component to the root entity
    auto *renderSettings = new Qt3DRender::QRenderSettings(rootEntity.data());
    rootEntity->addComponent(renderSettings);

    // Create the framegraph and attach it to the QRenderSettings
    createFrameGraph(renderSettings, &view, view.camera());

    // Create the scenegraph under the root entity
    createSceneGraph(rootEntity.data());

    // Set up the default camera
    auto *camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    auto *camController = new Qt3DExtras::QOrbitCameraController(rootEntity.data());
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.setRootEntity(rootEntity.data());
    view.show();
    return app.exec();
}
