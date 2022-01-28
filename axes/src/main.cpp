#include <QGuiApplication>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <QPropertyAnimation>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

#include "lineentity.h"

Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Line
    auto *lineEntity = new LineEntity(rootEntity);
    lineEntity->addVertex({0,0,0});
    lineEntity->addVertex({0,5,0});
    lineEntity->addVertex({5,5,0});
    lineEntity->addVertex({5,10,10});
    lineEntity->addIndices({0, 1, 2, 3, 0});
    lineEntity->finalise();
    auto *material = new Qt3DExtras::QPhongMaterial(lineEntity);
    material->setAmbient(Qt::blue);
    lineEntity->addComponent(material);

    return rootEntity;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    Qt3DCore::QEntity *scene = createScene();
    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));
    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);
    view.setRootEntity(scene);
    view.show();
    return app.exec();
}
