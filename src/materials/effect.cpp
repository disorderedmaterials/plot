#include "materials/effect.h"

#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QTechnique>
#include <QtCore/QUrl>

PhongEffect::PhongEffect(Qt3DCore::QNode *parent) : Qt3DRender::QEffect(parent)
{
    Q_INIT_RESOURCE(shaders);
    auto *filterKey = new Qt3DRender::QFilterKey(this);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    // Set up GLES 2.0 shader and technique
    //    auto *shader2 = new Qt3DRender::QShaderProgram();
    //    shader2->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/phong.vert"))));
    //    shader2->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/phong.frag"))));
    //
    //    auto *renderPass2 = new Qt3DRender::QRenderPass();
    //    renderPass2->setShaderProgram(shader2);
    //
    //    auto *techniqueES20 = new Qt3DRender::QTechnique();
    //    techniqueES20->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    //    techniqueES20->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGLES);
    //    techniqueES20->graphicsApiFilter()->setMajorVersion(2);
    //    techniqueES20->graphicsApiFilter()->setMinorVersion(0);
    //    techniqueES20->addFilterKey(filterKey);
    //    techniqueES20->addRenderPass(renderPass2);
    //    addTechnique(techniqueES20);

    // Set up GL 3.1 shader and technique
    auto *shader3 = new Qt3DRender::QShaderProgram(this);
    shader3->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/phong.vert"))));
    shader3->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/phong.frag"))));

    auto *renderPass3 = new Qt3DRender::QRenderPass(this);
    renderPass3->setShaderProgram(shader3);

    auto *techniqueGL31 = new Qt3DRender::QTechnique();
    techniqueGL31->addRenderPass(renderPass3);
    techniqueGL31->addFilterKey(filterKey);
    techniqueGL31->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    techniqueGL31->graphicsApiFilter()->setMajorVersion(3);
    techniqueGL31->graphicsApiFilter()->setMinorVersion(1);
    techniqueGL31->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    addTechnique(techniqueGL31);
}
