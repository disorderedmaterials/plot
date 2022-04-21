#include "materials/material.h"
#include <QVector3D>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <QtCore/QUrl>
#include <exception>

using namespace Mildred;

RenderableMaterial::RenderableMaterial(Qt3DCore::QNode *parent, VertexShaderType vertexShader,
                                       GeometryShaderType geometryShader, FragmentShaderType fragmentShader)
    : Qt3DRender::QMaterial(parent)
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

    // Set up GL 3.1 shader, render pass and technique
    auto *shader3 = new Qt3DRender::QShaderProgram(this);

    switch (vertexShader)
    {
        case (VertexShaderType::Unclipped):
            shader3->setVertexShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/unclipped.vert"))));
            break;
        case (VertexShaderType::ClippedToDataVolume):
            shader3->setVertexShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/clipped.vert"))));
            break;
        default:
            throw(std::runtime_error("Unhandled vertex shader type.\n"));
    }
    switch (geometryShader)
    {
        case (GeometryShaderType::None):
            break;
        case (GeometryShaderType::LineTesselator):
            shader3->setGeometryShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/line_tesselator.geom"))));
            break;
        default:
            throw(std::runtime_error("Unhandled geometry shader type.\n"));
    }
    switch (fragmentShader)
    {
        case (FragmentShaderType::Monochrome):
            shader3->setFragmentShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/monochrome.frag"))));
            break;
        case (FragmentShaderType::Phong):
            shader3->setFragmentShaderCode(
                Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/shaders/phong.frag"))));
            break;
        default:
            throw(std::runtime_error("Unhandled fragment shader type.\n"));
    }

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
 * General Properties
 */

// Set ambient colour component
void RenderableMaterial::setAmbient(QColor ambient)
{
    ambient_ = ambient;
    ambientParameter_->setValue(ambient_);
}

// Set diffuse colour component
void RenderableMaterial::setDiffuse(QColor diffuse)
{
    diffuse_ = diffuse;
    diffuseParameter_->setValue(diffuse_);
}

// Set specular colour component
void RenderableMaterial::setSpecular(QColor specular)
{
    specular_ = specular;
    specularParameter_->setValue(specular_);
}