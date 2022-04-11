#pragma once

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>

// Renderable Material
class RenderableMaterial : public Qt3DRender::QMaterial
{
    public:
    // Vertex Shader Types
    enum class VertexShaderType
    {
        Unclipped,
        ClippedToDataVolume
    };
    // Geometry Shader Types
    enum class GeometryShaderType
    {
        None,
        LineTesselator
    };
    // Fragment Shader Types
    enum class FragmentShaderType
    {
        Phong
    };
    explicit RenderableMaterial(Qt3DCore::QNode *parent, VertexShaderType vertexShader, GeometryShaderType geometryShader,
                                FragmentShaderType fragmentShader);

    /*
     * Properties
     */
    private:
    // Colour components
    QColor ambient_, diffuse_, specular_;
    // Shininess
    float shininess_{1.0};
    // Parameters
    Qt3DRender::QParameter *ambientParameter_{nullptr};
    Qt3DRender::QParameter *diffuseParameter_{nullptr};
    Qt3DRender::QParameter *specularParameter_{nullptr};
    Qt3DRender::QParameter *ambientReflectionParameter_{nullptr};
    Qt3DRender::QParameter *diffuseReflectionParameter_{nullptr};
    Qt3DRender::QParameter *specularReflectionParameter_{nullptr};
    Qt3DRender::QParameter *shininessParameter_{nullptr};

    public:
    // Set ambient colour component
    void setAmbient(QColor ambient);
    // Set diffuse colour component
    void setDiffuse(QColor diffuse);
    // Set specular colour component
    void setSpecular(QColor specular);
};
