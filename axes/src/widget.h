#pragma once

#include "framegraph.h"
#include <QWidget>
#include <Qt3DCore/QEntityPtr>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DWindow>

class MildredWidget : public QWidget
{
    Q_OBJECT

public:
    MildredWidget(QWidget *parent = nullptr);
    ~MildredWidget() = default;

    /*
     * Qt3D Objects
     */
    private:
    // Qt3DWindow for our display
    Qt3DExtras::Qt3DWindow *viewWindow_{nullptr};
    // Container widget for our Qt3DWindow
    QWidget *viewContainer_{nullptr};
    // Root entity containing framegraph and scenegraph
    Qt3DCore::QEntityPtr rootEntity_;
    // Render settings
    Qt3DRender::QRenderSettings *renderSettings_{nullptr};
    // Camera
    Qt3DRender::QCamera *camera_{nullptr};
    // Rendering framegraph
    MildredFrameGraph frameGraph_;

    /*
     * QWidget
     */
    protected:
    void resizeEvent(QResizeEvent *event) override;

    /*
     * SceneGraph
     */
    private:
    void createSceneGraph();
};
