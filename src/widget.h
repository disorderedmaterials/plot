#pragma once

#include "classes/metrics.h"
#include "entities/axis.h"
#include "entities/data1d.h"
#include "material.h"
#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QResizeEvent>
#include <QScopedPointer>
#include <QTimer>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QEntityPtr>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QInputSettings>
#include <Qt3DLogic/QFrameAction>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QRenderTarget>
#include <Qt3DRender/QRenderTargetOutput>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QViewport>
#include <QtOpenGLWidgets/QOpenGLWidget>

namespace Mildred
{
//! The Mildred widget is the core class of Mildred.
/*!
 * The Mildred widget is a standard QWidget displaying a Qt3D-based subwidget providing full 2D (flat) and 3D data
 * visualisation of discrete data as represented by the renderable data classes. It is the parent object of all displayed
 * entities, including axes.
 *
 * Look / feel of the display is controlled by a @class MildredMetrics object which most display classes retain a reference to
 * in order to have ready access to key metrics, e.g. the pixel scaling along each cardinal axis direction.
 */
class MildredWidget : public QOpenGLWidget
{
    Q_OBJECT

    public:
    explicit MildredWidget(QWidget *parent = nullptr);
    ~MildredWidget() override = default;

    /*
     * QOpenGLWidget Overrides
     */
    private:
    QScopedPointer<QOpenGLShaderProgram> glShaderProgram_;
    QOpenGLVertexArrayObject glVAO_;
    QOpenGLBuffer glVBO_;
    QVector<GLfloat> glVertexData_;
    int glVertexAttributeLocation_{0};
    int glTexCoordAttributeLocation_{0};
    bool initialised_{false};

    protected:
    void showEvent(QShowEvent *e) override;

    public:
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    public:
    void paintGL() override;

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
    // Offscreen framegraph
    QOffscreenSurface *offscreenSurface_{nullptr};
    Qt3DCore::QAspectEngine *aspectEngine_{nullptr};
    Qt3DRender::QRenderStateSet *renderStateSet_{nullptr};
    Qt3DRender::QDepthTest *depthTest_{nullptr};
    Qt3DRender::QMultiSampleAntiAliasing *multisampleAntialiasing_{nullptr};
    Qt3DRender::QRenderTargetSelector *renderTargetSelector_{nullptr};
    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector_{nullptr};
    Qt3DRender::QRenderTarget *renderTarget_{nullptr};
    Qt3DRender::QRenderTargetOutput *colourOutput_{nullptr};
    Qt3DRender::QTexture2D *colourTexture_{nullptr};
    Qt3DRender::QRenderTargetOutput *depthOutput_{nullptr};
    Qt3DRender::QTexture2D *depthTexture_{nullptr};
    Qt3DRender::QRenderSettings *renderSettings_{nullptr};
    Qt3DRender::QCamera *camera_{nullptr};
    // Root entity containing framegraph and scenegraph
    Qt3DCore::QEntityPtr rootEntity_;

    private:
    // Create framegraph
    void createFrameGraph();

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

    public:
    // Return x axis entity
    const AxisEntity *xAxis() const;
    // Return y axis entity
    const AxisEntity *yAxis() const;
    // Return z axis entity
    const AxisEntity *zAxis() const;

    public slots:
    // Update transforms from metrics
    void updateTransforms();
    // Update shader parameters
    void updateShaderParameters();
    // Reset view
    void resetView();
    // Axis Titles
    void setXAxisTitle(const QString &title);
    void setYAxisTitle(const QString &title);
    void setZAxisTitle(const QString &title);
    // Debug Objects
    void setSceneCuboidEnabled(bool enabled);

    /*
     * Mouse Handling / Interaction
     */
    private:
    // Coordinates of mouse down
    QPointF pressedMousePosition_;
    // Coordinates of mouse cursor
    QPointF lastMousePosition_;
    // Mouse release timer for detection of context menu requests
    QTimer mouseReleaseTimer_;
    // Current state of mouse buttons
    Qt::MouseButtons buttonState_;
    // Modifier state on mouse down
    Qt::KeyboardModifiers mouseDownModifiers_;

    private:
    // Mouse press event
    void mousePressEvent(QMouseEvent *event) override;
    // Mouse release event
    void mouseReleaseEvent(QMouseEvent *event) override;
    // Mouse move event
    void mouseMoveEvent(QMouseEvent *event) override;
    // Mouse wheel event
    void wheelEvent(QWheelEvent *event) override;
    // Mouse double click event
    void mouseDoubleClickEvent(QMouseEvent *event) override;

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
};
} // namespace Mildred
