#include "materials/monochrome_phong.h"
#include <QVector3D>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <QtCore/QUrl>

MonochromePhongMaterial::MonochromePhongMaterial(Qt3DCore::QNode *parent) : Qt3DRender::QMaterial(parent)
{
    // Initialise parameters
    ambient_.setNamedColor("#000000");
    diffuse_.setNamedColor("#CC6600");
    specular_.setNamedColor("#FFFFFF");
    ambientParameter_ = new Qt3DRender::QParameter(QStringLiteral("ambient"), ambient_, this);
    diffuseParameter_ = new Qt3DRender::QParameter(QStringLiteral("diffuse"), diffuse_, this);
    specularParameter_ = new Qt3DRender::QParameter(QStringLiteral("specular"), specular_, this);
    shininessParameter_ = new Qt3DRender::QParameter(QStringLiteral("shininess"), shininess_, this);
    ambientReflectionParameter_ = new Qt3DRender::QParameter(QStringLiteral("ka"), 1.0f, this);
    diffuseReflectionParameter_ = new Qt3DRender::QParameter(QStringLiteral("kd"), 1.0f, this);
    specularReflectionParameter_ = new Qt3DRender::QParameter(QStringLiteral("ks"), 1.0f, this);

    addParameter(ambientParameter_);
    addParameter(diffuseParameter_);
    addParameter(specularParameter_);
    addParameter(ambientReflectionParameter_);
    addParameter(diffuseReflectionParameter_);
    addParameter(specularReflectionParameter_);
    addParameter(shininessParameter_);

    auto *filterKey = new Qt3DRender::QFilterKey;
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    // Set up GL 3.1 shader and technique
    auto *shader3 = new Qt3DRender::QShaderProgram(this);
    shader3->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/standard.vert"))));
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

    auto *effect = new Qt3DRender::QEffect(this);
    effect->addTechnique(techniqueGL31);
    setEffect(effect);
}

/*
 * Properties
 */

// Set ambient colour component
void MonochromePhongMaterial::setAmbient(QColor ambient)
{
    ambient_ = ambient;
    ambientParameter_->setValue(ambient_);
}

// Set diffuse colour component
void MonochromePhongMaterial::setDiffuse(QColor diffuse)
{
    diffuse_ = diffuse;
    diffuseParameter_->setValue(diffuse_);
}

// Set specular colour component
void MonochromePhongMaterial::setSpecular(QColor specular)
{
    specular_ = specular;
    specularParameter_->setValue(specular_);
}