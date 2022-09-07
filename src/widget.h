#pragma once

#include "classes/metrics.h"
#include "displaygroup.h"
#include "entities/axis.h"
#include "entities/data1d.h"
#include "framegraph.h"
#include "material.h"
#include <QResizeEvent>
#include <QScopedPointer>
#include <QTimer>
#include <QWidget>
#include <Qt3DCore/QEntityPtr>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DInput/QKeyEvent>
#include <Qt3DInput/QMouseEvent>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

namespace Mildred
{
// Coordinate Display
enum class CoordinateDisplayStyle
{
    None,
    MouseAnchor,
    FixedAnchor
};

//! The Mildred widget is the core class of Mildred.
/*!
 * The Mildred widget is a standard QWidget displaying a Qt3D-based subwidget providing full 2D (flat) and 3D data
 * visualisation of discrete data as represented by the renderable data classes. It is the parent object of all displayed
 * entities, including axes.
 *
 * Look / feel of the display is controlled by a @class MildredMetrics object which most display classes retain a reference to
 * in order to have ready access to key metrics, e.g. the pixel scaling along each cardinal axis direction.
 */
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

    private slots:
    // Update metrics for current surface size
    void updateMetrics();

    /*
     * Appearance
     */
    private:
    // Whether the current view is flat
    bool flatView_{true};

    public:
    // Return whether the view is flat
    bool isFlatView() const;

    public slots:
    // Set whether view is flat
    void setFlatView(bool flat);

    /*
     * FrameGraph
     */
    private:
    MildredFrameGraph framegraph_;

    /*
     * SceneGraph
     */
    private:
    // Global light position
    QVector3D lightPosition_{0.0, 0.0, -100.0};
    // View rotation
    QQuaternion viewRotationMatrix_;
    // Head node for scene (owned by root entity)
    Qt3DCore::QEntity *sceneRootEntity_{nullptr};
    // Axes
    AxisEntity *xAxis_{nullptr}, *yAxis_{nullptr}, *altYAxis_{nullptr}, *zAxis_{nullptr};
    // Transforms and associated parameters
    Qt3DCore::QTransform *dataOriginTransform_{nullptr}, *sceneObjectsTransform_{nullptr}, *sceneRootTransform_{nullptr};
    // Data Entities
    Qt3DCore::QEntity *dataRootEntity_{nullptr}, *dataEntityParent_{nullptr};
    // Debug objects
    Qt3DCore::QEntity *sceneBoundingCuboidEntity_{nullptr};
    Qt3DCore::QTransform *sceneBoundingCuboidTransform_{nullptr};
    // Shader parameters
    Qt3DRender::QParameter *sceneDataAxesParameter_{nullptr};
    Qt3DRender::QParameter *sceneDataAxesExtentsParameter_{nullptr};
    Qt3DRender::QParameter *sceneDataAxesOriginParameter_{nullptr};
    Qt3DRender::QParameter *sceneDataTransformInverseParameter_{nullptr};
    Qt3DRender::QParameter *viewportSizeParameter_{nullptr};

    private:
    // Create basic scenegraph
    void createSceneGraph();
    // Convert widget position to 2D (flat) coordinates
    QPointF toAxes2D(QPoint pos) const;
    // Return screen coordinates at centre of 2D view
    QPoint screen2DCentre() const;

    public:
    // Return x axis entity
    AxisEntity *xAxis();
    // Return y axis entity
    AxisEntity *yAxis();
    // Return z axis entity
    AxisEntity *zAxis();

    public slots:
    // Update transforms from metrics
    void updateTransforms();
    // Update shader parameters
    void updateShaderParameters();
    // Reset view
    void resetView();
    // Show all data in view
    void showAllData();
    // Axis Main Labels
    void setXAxisLabel(const QString &text);
    void setYAxisLabel(const QString &text);
    void setZAxisLabel(const QString &text);
    void setXAxisLabelRotation(double theta);
    void setYAxisLabelRotation(double theta);
    void setZAxisLabelRotation(double theta);
    // Debug Objects
    void setSceneCuboidEnabled(bool enabled);

    /*
     * Mouse Handling / Interaction
     */
    private:
    // Last recorded mouse position
    QPoint lastMousePosition_;
    // Mouse coordinate style
    CoordinateDisplayStyle mouseCoordStyle_{CoordinateDisplayStyle::FixedAnchor};
    // Mouse coordinate entity
    TextEntity *mouseCoordEntity_{nullptr};

    private slots:
    void mousePositionChanged(Qt3DInput::QMouseEvent *event);
    void mouseButtonPressed(Qt3DInput::QMouseEvent *event);
    void mouseButtonReleased(Qt3DInput::QMouseEvent *event);
    void mouseWheeled(Qt3DInput::QWheelEvent *event);

    public slots:
    void setMouseCoordStyle(CoordinateDisplayStyle style);

    signals:
    void mouseCoordChanged(QPointF pos);

    /*
     * Keyboard Handling / Interaction
     */
    private slots:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    /*
     * Display Data
     */
    private:
    // Associated data entities (with identifying tag)
    std::vector<std::pair<std::string, DataEntity *>> dataEntities_;

    private:
    // Create material for specified entity
    RenderableMaterial *createMaterial(
        Qt3DCore::QEntity *parent,
        RenderableMaterial::VertexShaderType vertexShader = RenderableMaterial::VertexShaderType::ClippedToDataVolume,
        RenderableMaterial::GeometryShaderType geometryShader = RenderableMaterial::GeometryShaderType::None,
        RenderableMaterial::FragmentShaderType fragmentShader = RenderableMaterial::FragmentShaderType::Phong);

    public:
    // Add new data entity for supplied data
    Data1DEntity *addData1D(std::string_view tag);

    /*
     * Grouping
     */
    private:
    // Defined display groups
    std::vector<std::shared_ptr<DisplayGroup>> displayGroups_;

    public:
    // Create new display group
    DisplayGroup *addDisplayGroup();
};
} // namespace Mildred