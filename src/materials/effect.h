#pragma once

#include <Qt3DRender/QEffect>

class PhongEffect : public Qt3DRender::QEffect
{
public:
    explicit PhongEffect(Qt3DCore::QNode *parent = nullptr);
};
