#pragma once

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

//! MildredFrameGraph creates and manages a framegraph
class MildredFrameGraph
{
    public:
    MildredFrameGraph() = default;
    ~MildredFrameGraph() = default;

    /*
     * Qt3D Objects
     */
    private:
    // Parent QRenderSettings
    Qt3DRender::QRenderSettings *renderSettings_{nullptr};

    public:
    // Create and attach framegraph
    void create(Qt3DRender::QRenderSettings *parent, Qt3DExtras::Qt3DWindow *surface, Qt3DRender::QCamera *camera);
};
