#pragma once

#include "axisentity.h"
#include "framegraph.h"
#include "metrics.h"
#include <QWidget>
#include <Qt3DCore/QEntityPtr>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DInput/QMouseEvent>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

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
    // Widget resized
    void resizeEvent(QResizeEvent *event) override;

    /*
     * Metrics
     */
    private:
    // Display metrics
    MildredMetrics metrics_;

    private:
    // Update metrics for specified surface size
    void updateMetrics(int width, int height);

    public:
    // Update metrics for current surface size
    void updateMetrics();

    /*
     * Appearance
     */
    public:
    // View Types
    enum class ViewType
    {
        View2D,
        View3D
    };

    /*
     * SceneGraph
     */
    private:
    // Head node for scene (owned by root entity)
    Qt3DCore::QEntity *sceneRootEntity_{nullptr};
    // Axes
    AxisEntity *xAxis_{nullptr}, *yAxis_{nullptr}, *altYAxis_{nullptr}, *zAxis_{nullptr};
    // Transforms
    Qt3DCore::QTransform *dataOriginTransform_{nullptr}, *sceneObjectsTransform_{nullptr}, *sceneRootTransform_{nullptr};
    // Debug objects
    Qt3DCore::QEntity *sceneBoundingCuboidEntity_{nullptr};
    Qt3DCore::QTransform *sceneBoundingCuboidTransform_{nullptr};

    private:
    // Create basic scenegraph
    void createSceneGraph();

    public:
    // Return x axis entity
    const AxisEntity *xAxis() const;
    // Return y axis entity
    const AxisEntity *yAxis() const;
    // Return z axis entity
    const AxisEntity *zAxis() const;

    /*
     * Mouse Handling / Interaction
     */
    private:
    // Last recorded mouse position
    QPoint lastMousePosition_;

    private slots:
    void mousePositionChanged(Qt3DInput::QMouseEvent *event);
    void mouseButtonPressed(Qt3DInput::QMouseEvent *event);
    void mouseButtonReleased(Qt3DInput::QMouseEvent *event);

    /*
     * Slots
     */
    public slots:
    // Axis Titles
    void setXAxisTitle(const QString &title);
    void setYAxisTitle(const QString &title);
    void setZAxisTitle(const QString &title);
    // Debug Objects
    void setSceneCuboidEnabled(bool enabled);
};
