#pragma once

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>

// Phong Material
class PhongMaterial : public Qt3DRender::QMaterial
{
public:
    explicit PhongMaterial(Qt3DCore::QNode *parent = nullptr);

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
};
