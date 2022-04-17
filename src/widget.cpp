#include "widget.h"
#include "entities/axis.h"
#include "materials/material.h"
#include <QOpenGLFunctions>
#include <QResizeEvent>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

// Initialise Qt resources
void initialiseQtResources() { Q_INIT_RESOURCE(shaders); }

using namespace Mildred;

//! Constructs a Mildred widget which is a child of \param parent.
MildredWidget::MildredWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // Initialise resources
    initialiseQtResources();

    //    viewWindow_ = new Qt3DExtras::Qt3DWindow();

    // Create a container for the Qt3DWindow
    //    viewContainer_ = createWindowContainer(viewWindow_, this);

    // Create our root entity
    rootEntity_ = Qt3DCore::QEntityPtr(new Qt3DCore::QEntity);

    // Create a QRenderSettings and add it as a component to the root entity
    //    renderSettings_ = viewWindow_->renderSettings();

    // Create parameters
    sceneDataAxesParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxes"), QMatrix4x4());
    sceneDataAxesExtentsParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesExtents"), QVector3D());
    sceneDataAxesOriginParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesOrigin"), QVector3D());
    sceneDataTransformInverseParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataTransformInverse"), QMatrix4x4());
    viewportSizeParameter_ = new Qt3DRender::QParameter(QStringLiteral("viewportSize"), QVector2D());

    // Add a mouse handler and connect it up
    auto *mouseHandler = new Qt3DInput::QMouseHandler(rootEntity_.data());
    auto *mouseDevice = new Qt3DInput::QMouseDevice(rootEntity_.data());
    mouseHandler->setSourceDevice(mouseDevice);
    rootEntity_->addComponent(mouseHandler);
    connect(mouseHandler, SIGNAL(positionChanged(Qt3DInput::QMouseEvent *)), this,
            SLOT(mousePositionChanged(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(pressed(Qt3DInput::QMouseEvent *)), this, SLOT(mouseButtonPressed(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent *)), this,
            SLOT(mouseButtonReleased(Qt3DInput::QMouseEvent *)));

    /*
     * The framegraph is constructed with the following structure:
     *
     *        [QRenderSettings]         Parent of first framegraph node
     *               |
     *        QSurfaceSelector          Sets the target surface for the renderer
     *               |
     *           QViewport              Defines the viewport on the target surface
     *               |
     *         QClearBuffers            Sets the targets to clear and the default attributes to clear to
     *               |
     *        QCameraSelector           Selects an existing camera to view the scenegraph
     */

    // Set up our offscreen rendering surface
    int samples = QSurfaceFormat::defaultFormat().samples();
    offscreenSurface_ = new QOffscreenSurface;
    offscreenSurface_->setFormat(QSurfaceFormat::defaultFormat());
    offscreenSurface_->create();

    aspectEngine_ = new Qt3DCore::QAspectEngine;
    aspectEngine_->registerAspect(new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Automatic));
    aspectEngine_->registerAspect(new Qt3DInput::QInputAspect);
    aspectEngine_->registerAspect(new Qt3DLogic::QLogicAspect);

    // Setup color
    colourOutput_ = new Qt3DRender::QRenderTargetOutput;
    colourOutput_->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

    // Create a color texture to render into
    colourTexture_ = new Qt3DRender::QTexture2DMultisample;
    colourTexture_->setSize(width(), height());
    colourTexture_->setFormat(Qt3DRender::QAbstractTexture::RGB8_UNorm);
    colourTexture_->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    colourTexture_->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);

    // Hook the texture up to our output, and the output up to this object
    colourOutput_->setTexture(colourTexture_);
    colourTexture_->setSamples(samples);
    renderTarget_ = new Qt3DRender::QRenderTarget;
    renderTarget_->addOutput(colourOutput_);

    // Setup depth
    depthOutput_ = new Qt3DRender::QRenderTargetOutput;
    depthOutput_->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Depth);

    // Create depth texture
    depthTexture_ = new Qt3DRender::QTexture2DMultisample;
    depthTexture_->setSize(width(), height());
    depthTexture_->setFormat(Qt3DRender::QAbstractTexture::DepthFormat);
    depthTexture_->setMinificationFilter(Qt3DRender::QAbstractTexture::Linear);
    depthTexture_->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    depthTexture_->setComparisonFunction(Qt3DRender::QAbstractTexture::CompareLessEqual);
    depthTexture_->setComparisonMode(Qt3DRender::QAbstractTexture::CompareRefToTexture);

    // Hook up the depth texture
    depthOutput_->setTexture(depthTexture_);
    depthTexture_->setSamples(samples);
    renderTarget_->addOutput(depthOutput_);

    // Create our render state set
    renderStateSet_ = new Qt3DRender::QRenderStateSet;
    renderStateSet_->addRenderState(new Qt3DRender::QMultiSampleAntiAliasing);
    // -- Depth test
    depthTest_ = new Qt3DRender::QDepthTest;
    depthTest_->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet_->addRenderState(depthTest_);
    // -- Enable six clip planes for the data viewing volume
    for (auto n = 0; n < 6; ++n)
    {
        auto *cp = new Qt3DRender::QClipPlane;
        cp->setPlaneIndex(n);
        cp->setEnabled(true);
        renderStateSet_->addRenderState(cp);
    }
    // -- Face culling
    auto *cull = new Qt3DRender::QCullFace();
    cull->setMode(Qt3DRender::QCullFace::NoCulling);
    renderStateSet_->addRenderState(cull);

    // Render target selector
    renderTargetSelector_ = new Qt3DRender::QRenderTargetSelector;
    renderTargetSelector_->setParent(renderStateSet_);
    renderTargetSelector_->setTarget(renderTarget_);

    // Render surface selector
    renderSurfaceSelector_ = new Qt3DRender::QRenderSurfaceSelector;
    renderSurfaceSelector_->setSurface(offscreenSurface_);
    renderSurfaceSelector_->setParent(renderTargetSelector_);

    // Viewport
    auto *viewport = new Qt3DRender::QViewport(renderSurfaceSelector_);
    viewport->setNormalizedRect({0.0, 0.0, 1.0, 1.0});

    // Clear to background colour
    auto *clearBuffers = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(QColor(255, 255, 255, 255));

    // Construct a camera
    camera_ = new Qt3DRender::QCamera;
    //    camera_->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setPosition(QVector3D(0, 0, 1.0f));
    camera_->setViewCenter(QVector3D(0, 0, -10.0));
    auto *cameraSelector = new Qt3DRender::QCameraSelector(clearBuffers);
    cameraSelector->setCamera(camera_);

    auto *forwardRenderer_ = new Qt3DExtras::QForwardRenderer;
    forwardRenderer_->setCamera(camera_);
    forwardRenderer_->setSurface(offscreenSurface_);
    forwardRenderer_->setParent(camera_);
    forwardRenderer_->setClearColor("blue");

    // Render settings, pointing to the top of the framegraph
    renderSettings_ = new Qt3DRender::QRenderSettings;
    renderSettings_->setActiveFrameGraph(renderStateSet_);

    inputSettings_ = new Qt3DInput::QInputSettings;
    inputSettings_->setEventSource(this);

    // Set up basic scenegraph
    createSceneGraph();

    // Create a frame action so we can receive "render complete" notifications
    frameAction_ = new Qt3DLogic::QFrameAction;

    // Set the main root entity
    //    setRootEntity(rootEntity_.data());

    // Connect the metrics object and update
    connect(&metrics_, SIGNAL(metricsChanged()), this, SLOT(updateTransforms()));
}

/*
 * QOpenGLWidget Overrides
 */

void MildredWidget::showEvent(QShowEvent *e)
{
    if (!initialised_)
    {
        rootEntity_->addComponent(renderSettings_);
        rootEntity_->addComponent(inputSettings_);
        rootEntity_->addComponent(frameAction_);
        connect(frameAction_, &Qt3DLogic::QFrameAction::triggered, [this]() { this->update(); });
        aspectEngine_->setRootEntity(rootEntity_);

        initialised_ = true;
    }

    QWidget::showEvent(e);
}

//! Handle QWidget resize events
/*!
 * Resizing the widget demands that the metrics information held in @class MildredMetrics is updated, ensuring the whole of the
 * available drawing surface is used for visualisation.
 */
void MildredWidget::resizeEvent(QResizeEvent *event)
{
    updateMetrics();

    // Move the scene root position to be the centre of the XY plane and a suitable distance away
    sceneRootTransform_->setTranslation(QVector3D(width() / 2.0, height() / 2.0, -width()));

    // Reset projection for new viewport
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, width() * 2.0f);
    camera_->setAspectRatio(float(width()) / float(height()));

    // Debug objects
    sceneBoundingCuboidTransform_->setScale3D(QVector3D(width(), height(), width()));

    // Lastly, resize our view container
    //    viewContainer_->resize(this->size());

    colourTexture_->setSize(width(), height());
    depthTexture_->setSize(width(), height());
    renderSurfaceSelector_->setExternalRenderTargetSize(QSize(width(), height()));
}

void MildredWidget::initializeGL()
{
    static const int coords[4][3] = {{+1, 0, 0}, {0, 0, 0}, {0, +1, 0}, {+1, +1, 0}};

    for (auto i = 0; i < 4; ++i)
    {
        // vertex position
        glVertexData_.append(coords[i][0]);
        glVertexData_.append(coords[i][1]);
        glVertexData_.append(coords[i][2]);
        // texture coordinate
        glVertexData_.append(i == 0 || i == 3);
        glVertexData_.append(i == 0 || i == 1);
    }

    // Setup our vertex array object. We later only need to bind this
    // to be able to draw.
    glVAO_.create();
    // The binder automatically binds the vao and unbinds it at the end
    // of the function.
    QOpenGLVertexArrayObject::Binder vaoBinder(&glVAO_);

    // Setup our vertex buffer object.
    glVBO_.create();
    glVBO_.bind();
    glVBO_.allocate(glVertexData_.constData(), glVertexData_.count() * sizeof(GLfloat));

    glVBO_.bind();
    auto *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(glVertexAttributeLocation_);
    f->glEnableVertexAttribArray(glTexCoordAttributeLocation_);
    f->glVertexAttribPointer(glVertexAttributeLocation_, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(glTexCoordAttributeLocation_, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glVBO_.release();

    glShaderProgram_.reset(new QOpenGLShaderProgram);
    glShaderProgram_->addShaderFromSourceCode(QOpenGLShader::Vertex, "#version 150\n"
                                                                     "in highp vec3 vertex;\n"
                                                                     "in mediump vec2 texCoord;\n"
                                                                     "out mediump vec2 texc;\n"
                                                                     "uniform mediump mat4 matrix;\n"
                                                                     "void main(void)\n"
                                                                     "{\n"
                                                                     "        gl_Position = matrix * vec4(vertex, 1.0);\n"
                                                                     "        texc = texCoord;\n"
                                                                     "}\n");
    glShaderProgram_->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                              "#version 150\n"
                                              "uniform sampler2DMS texture;\n"
                                              "in mediump vec2 texc;\n"
                                              "uniform int samples;\n"
                                              "void main(void)\n"
                                              "{\n"
                                              "   ivec2 tc = ivec2(floor(textureSize(texture) * texc));\n"
                                              "   vec4 color = vec4(0.0);\n"
                                              "   for(int i = 0; i < samples; i++) {\n"
                                              "       color += texelFetch(texture, tc, i);\n"
                                              "   }\n"
                                              "   gl_FragColor = color / float(samples);\n"
                                              "}\n");
    glShaderProgram_->bindAttributeLocation("vertex", glVertexAttributeLocation_);
    glShaderProgram_->bindAttributeLocation("texCoord", glTexCoordAttributeLocation_);
    glShaderProgram_->link();

    glShaderProgram_->bind();
    glShaderProgram_->setUniformValue("texture", 0);
    glShaderProgram_->setUniformValue("samples", QSurfaceFormat::defaultFormat().samples());
    glShaderProgram_->release();
}

void MildredWidget::resizeGL(int w, int h)
{
    updateMetrics();

    // Move the scene root position to be the centre of the XY plane and a suitable distance away
    sceneRootTransform_->setTranslation(QVector3D(w / 2.0, h / 2.0, -w));

    camera_->lens()->setOrthographicProjection(0, w, 0, h, 0.1f, width() * 2.0f);
    camera_->setAspectRatio(float(w) / float(h));
    colourTexture_->setSize(w, h);
    depthTexture_->setSize(w, h);
    renderSurfaceSelector_->setExternalRenderTargetSize(QSize(w, h));
}

void MildredWidget::paintGL()
{
    printf("jslkdjaslkd\n");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glShaderProgram_->bind();
    {
        QMatrix4x4 m;
        m.ortho(0, 1, 0, 1, 1.0f, 3.0f);
        m.translate(0.0f, 0.0f, -2.0f);

        QOpenGLVertexArrayObject::Binder vaoBinder(&glVAO_);

        glShaderProgram_->setUniformValue("matrix", m);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colourTexture_->handle().toUInt());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glShaderProgram_->release();
}

/*
 * Metrics
 */

//! Update metrics for current surface size
/*!
 *  Updates the internal @class MildredMetrics object.
 */
void MildredWidget::updateMetrics() { metrics_.update(width(), height(), xAxis_, yAxis_); }

/*
 * Appearance
 */

//! Return whether the view is flat
bool MildredWidget::isFlatView() const { return flatView_; }

//! Set whether view is flat
/*!
 * This controls whether the current view is set to fixed, flat (2D) drawing (@param flat = true) with only the x and y axes
 * shown, or the view is full 3D in which case all axes (including the depth z axis) are visible.
 *
 * Changing the view type necessarily enforces a recalculation of the metrics object.
 */
void MildredWidget::setFlatView(bool flat)
{
    if (flatView_ == flat)
        return;

    flatView_ = flat;

    // Set z-axis visibility
    zAxis_->setEnabled(!flatView_);

    // Reset view and update
    resetView();
    updateMetrics();
}

/*
 * SceneGraph
 */

//! Creates the scene graph for the display.
/*!
 * The Qt3D scene graph is created with the following layout.
 *
 *          [RootEntity]               Top-level root entity
 *               |
 *         sceneRootEntity_
 *          |           |
 * sceneRootTransform_  |              Translates to distant rotation point and applies 3D view rotation
 *                      |
 *              sceneObjectsEntity_    Contains all viewable objects for plotting
 *               |         |    |
 * sceneObjectsTransform_  |    |      Places scene objects so that global 0,0,0 is lower left corner to the viewer
 *                         |    |
 *                 axesEntity   |      Contains the individual AxesEntities
 *                     |        |
 *           xAxis_,yAxis_...   |      Individual axis entities
 *                              |
 *                      dataEntity     Parent entity for all displayed data series
 *                       |      |
 *      dataOriginTransform_    *      Sets the coordinate system so that 0,0,0 is at the axes origin
 */
void MildredWidget::createSceneGraph()
{
    auto *lightEntity = new Qt3DCore::QEntity(rootEntity_.data());

    auto *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);

    auto *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(lightPosition_);
    lightEntity->addComponent(lightTransform);

    sceneRootEntity_ = new Qt3DCore::QEntity(rootEntity_.data());
    sceneRootTransform_ = new Qt3DCore::QTransform(sceneRootEntity_);
    sceneRootEntity_->addComponent(sceneRootTransform_);

    auto *sceneObjectsEntity_ = new Qt3DCore::QEntity(sceneRootEntity_);
    sceneObjectsTransform_ = new Qt3DCore::QTransform(sceneObjectsEntity_);
    sceneObjectsEntity_->addComponent(sceneObjectsTransform_);

    // Debug
    sceneBoundingCuboidEntity_ = new Qt3DCore::QEntity(sceneRootEntity_);
    sceneBoundingCuboidEntity_->setEnabled(false);
    auto *cuboidMesh = new Qt3DExtras::QCuboidMesh(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->addComponent(cuboidMesh);
    sceneBoundingCuboidTransform_ = new Qt3DCore::QTransform(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->addComponent(sceneBoundingCuboidTransform_);
    auto *cuboidMaterial = new Qt3DExtras::QDiffuseSpecularMaterial(sceneBoundingCuboidEntity_);
    cuboidMaterial->setAmbient(QColor(255, 0, 0, 100));
    cuboidMaterial->setAlphaBlendingEnabled(true);
    sceneBoundingCuboidEntity_->addComponent(cuboidMaterial);

    /*
     * Axes Leaf
     */

    auto *axesEntity = new Qt3DCore::QEntity(sceneObjectsEntity_);

    auto *xAxisBarMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::LineTesselator, RenderableMaterial::FragmentShaderType::Phong);
    xAxisBarMaterial->setAmbient(QColor(0, 0, 0, 255));
    auto *xAxisLabelMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Monochrome);
    xAxisLabelMaterial->setAmbient(QColor(0, 0, 0, 255));
    xAxis_ = new AxisEntity(axesEntity, AxisEntity::AxisType::Horizontal, metrics_, xAxisBarMaterial, xAxisLabelMaterial);
    connect(xAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));

    auto *yAxisBarMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::LineTesselator, RenderableMaterial::FragmentShaderType::Phong);
    yAxisBarMaterial->setAmbient(QColor(0, 0, 0, 255));
    auto *yAxisLabelMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Monochrome);
    yAxisLabelMaterial->setAmbient(QColor(0, 0, 0, 255));
    yAxis_ = new AxisEntity(axesEntity, AxisEntity::AxisType::Vertical, metrics_, yAxisBarMaterial, yAxisLabelMaterial);
    connect(yAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));

    auto *zAxisBarMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::LineTesselator, RenderableMaterial::FragmentShaderType::Phong);
    zAxisBarMaterial->setAmbient(QColor(0, 0, 0, 255));
    auto *zAxisLabelMaterial =
        createMaterial(axesEntity, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Monochrome);
    zAxisLabelMaterial->setAmbient(QColor(0, 0, 0, 255));
    zAxis_ = new AxisEntity(axesEntity, AxisEntity::AxisType::Depth, metrics_, zAxisBarMaterial, zAxisLabelMaterial);
    connect(zAxis_, SIGNAL(enabledChanged(bool)), this, SLOT(updateMetrics()));
    zAxis_->setEnabled(false);

    /*
     * Data Space Leaf
     */

    dataRootEntity_ = new Qt3DCore::QEntity(sceneObjectsEntity_);
    dataEntityParent_ = new Qt3DCore::QEntity(dataRootEntity_);
    dataOriginTransform_ = new Qt3DCore::QTransform(dataEntityParent_);
    dataEntityParent_->addComponent(dataOriginTransform_);

    // TEST
    auto *sphereEntity_ = new Qt3DCore::QEntity(dataEntityParent_);
    auto *sphereMesh = new Qt3DExtras::QSphereMesh(sphereEntity_);
    sphereEntity_->addComponent(sphereMesh);
    createMaterial(sphereEntity_);
    auto *sphereTransform = new Qt3DCore::QTransform();
    sphereTransform->setScale(50.0);
    sphereTransform->setTranslation(QVector3D(530.0, 50, 0));
    sphereEntity_->addComponent(sphereTransform);
}

//! Return x axis entity
const AxisEntity *MildredWidget::xAxis() const { return xAxis_; }

//! Return y axis entity
const AxisEntity *MildredWidget::yAxis() const { return yAxis_; }

//! Return z axis entity
const AxisEntity *MildredWidget::zAxis() const { return zAxis_; }

//! Update transforms from metrics
/*!
 * The metrics defined in the @class MildredMetrics object contains the offsets in 3D space for the axes origin and general
 * graphing volume. This slot is connected internally to signals in the @class MildredMetrics object to ensure correct update of
 * the view when the metrics have been recalculated.
 */
void MildredWidget::updateTransforms()
{
    if (sceneObjectsTransform_)
        sceneObjectsTransform_->setTranslation(metrics_.displayVolumeOrigin() -
                                               QVector3D(width() / 2.0, height() / 2.0, -width() / 2.0));
    if (dataOriginTransform_)
        dataOriginTransform_->setTranslation(
            -(xAxis_->toScaled(xAxis_->minimum()) + yAxis_->toScaled(yAxis_->minimum()) + zAxis_->toScaled(zAxis_->minimum())));
}

//! Update shader parameters
/*!
 * Update shader parameters from current view information.
 */
void MildredWidget::updateShaderParameters()
{
    sceneDataAxesParameter_->setValue(QMatrix4x4(xAxis_->direction().x(), xAxis_->direction().y(), xAxis_->direction().z(), 0.0,
                                                 yAxis_->direction().x(), yAxis_->direction().y(), yAxis_->direction().z(), 0.0,
                                                 zAxis_->direction().x(), zAxis_->direction().y(), zAxis_->direction().z(), 0.0,
                                                 0.0, 0.0, 0.0, 1.0));
    sceneDataAxesExtentsParameter_->setValue(metrics_.displayVolumeExtent());
    sceneDataAxesOriginParameter_->setValue(xAxis_->toScaled(xAxis_->minimum()) + yAxis_->toScaled(yAxis_->minimum()) +
                                            zAxis_->toScaled(zAxis_->minimum()));
    sceneDataTransformInverseParameter_->setValue(
        (sceneRootTransform_->matrix() * sceneObjectsTransform_->matrix() * dataOriginTransform_->matrix()).inverted());
    viewportSizeParameter_->setValue(QVector2D(width(), height()));
}

//! Reset view
/*!
 * Resets the main scene view rotation to the identity matrix
 */
void MildredWidget::resetView()
{
    assert(sceneRootTransform_);

    viewRotationMatrix_ = QQuaternion();
    sceneRootTransform_->setRotation(viewRotationMatrix_);

    updateShaderParameters();
}

/*
 * Mouse Handling
 */

//! React to mouse movement
/*!
 * React to mouse move events signalled by a Qt3DMouseHandler attached to the Qt3D surface, potentially affecting changes to the
 * axes ranges or view volume, depending on the current view type and options.
 *
 * Active mouse buttons modify the effect of the event, and which also depends on the current view mode and depressed modifier
 * keys.
 *
 * | Button | View Type | Modifier | Action |
 * | :----: | :-------: | :------: | ------ |
 * | Left   | Flat / 2D | None     | Update current coordinate under the mouse. |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     | None   |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | Middle | Flat / 2D | None     | Translate view in the XY plane, modifying the corresponding ranges of the x and y axes |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     |        |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | Right  | Flat / 2D | None     | None   |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 * | ^      | 3D        | None     | Rotate view volume around its centroid. |
 * | ^      | ^         | Ctrl     |        |
 * | ^      | ^         | Shift    |        |
 */
void MildredWidget::mousePositionChanged(Qt3DInput::QMouseEvent *event)
{
    // Check previous position
    if (lastMousePosition_.isNull())
    {
        lastMousePosition_ = QPoint(event->x(), event->y());
        return;
    }

    // Right button - Rotate scene volume (3D)
    if (event->buttons() & Qt3DInput::QMouseEvent::RightButton)
    {
        // Rotations only allowed for 3D view
        if (!flatView_)
        {
            viewRotationMatrix_ *=
                QQuaternion::fromEulerAngles(event->y() - lastMousePosition_.y(), event->x() - lastMousePosition_.x(), 0.0);
            sceneRootTransform_->setRotation(viewRotationMatrix_);
        }
    }

    // Middle button - translate axis ranges (2D)
    if (event->buttons() & Qt3DInput::QMouseEvent::MiddleButton)
    {
        if (flatView_)
        {
            xAxis_->shiftLimits(-xAxis_->range() *
                                (double(event->x() - lastMousePosition_.x()) / metrics_.displayVolumeExtent().x()));
            yAxis_->shiftLimits(yAxis_->range() *
                                (double(event->y() - lastMousePosition_.y()) / metrics_.displayVolumeExtent().y()));
            updateTransforms();
        }
    }

    updateShaderParameters();

    lastMousePosition_ = QPoint(event->x(), event->y());
}

void MildredWidget::mouseButtonPressed(Qt3DInput::QMouseEvent *event) {}

void MildredWidget::mouseButtonReleased(Qt3DInput::QMouseEvent *event) {}

/*
 * Display Data
 */

//! Create material for specified entity
/*!
 * Create and attach a new RenderableMaterial to the specified @param parent, with the specified @param vertexShader, @param
 * geometryShader, and @param fragmentShader.
 */
RenderableMaterial *MildredWidget::createMaterial(Qt3DCore::QEntity *parent, RenderableMaterial::VertexShaderType vertexShader,
                                                  RenderableMaterial::GeometryShaderType geometryShader,
                                                  RenderableMaterial::FragmentShaderType fragmentShader)
{
    auto *material = new RenderableMaterial(parent, vertexShader, geometryShader, fragmentShader);

    // Attach necessary parameters
    material->addParameter(sceneDataAxesParameter_);
    material->addParameter(sceneDataAxesExtentsParameter_);
    material->addParameter(sceneDataAxesOriginParameter_);
    material->addParameter(sceneDataTransformInverseParameter_);
    material->addParameter(viewportSizeParameter_);

    // Add the material as a component on the parent
    parent->addComponent(material);

    return material;
}

// Add new 1-dimensional data entity for supplied data
Data1DEntity *MildredWidget::addData1D(std::string_view tag)
{
    // Check for existing tag
    auto it = std::find_if(dataEntities_.begin(), dataEntities_.end(), [tag](const auto &d) { return tag == d.first; });
    if (it != dataEntities_.end())
    {
        printf("Data with tag '%s' already exists, so can't add it again. Returning the existing data instead.\n",
               it->first.c_str());
        throw(std::runtime_error("Duplicate DataEntity tag created.\n"));
    }

    // Create a new entity
    auto *entity = new Data1DEntity(xAxis_, yAxis_, dataEntityParent_);
    connect(&metrics_, SIGNAL(metricsChanged()), entity, SLOT(updateRenderables()));
    dataEntities_.emplace_back(tag, entity);

    // Add a material (testing for now)
    auto *material = createMaterial(entity, RenderableMaterial::VertexShaderType::ClippedToDataVolume,
                                    RenderableMaterial::GeometryShaderType::LineTesselator);
    material->setAmbient(Qt::blue);
    material->addParameter(new Qt3DRender::QParameter(QStringLiteral("lineWidth"), 1.5f));
    entity->setDataMaterial(material);

    return entity;
}

/*
 * Slots
 */

//! Change the text of the x-axis title.
/*!
 * Set the displayed text of the x-axis title to @param title. Metrics are updated after the change.
 */
void MildredWidget::setXAxisTitle(const QString &title)
{
    assert(xAxis_);
    xAxis_->setTitleText(title);
    updateMetrics();
}

//! Change the text of the y-axis title.
/*!
 * Set the displayed text of the y-axis title to @param title. Metrics are updated after the change.
 */
void MildredWidget::setYAxisTitle(const QString &title)
{
    assert(yAxis_);
    yAxis_->setTitleText(title);
    updateMetrics();
}

//! Change the text of the z-axis title.
/*!
 * Set the displayed text of the z-axis title to @param title. Metrics are updated after the change.
 */
void MildredWidget::setZAxisTitle(const QString &title)
{
    assert(zAxis_);
    zAxis_->setTitleText(title);
    updateMetrics();
}

//! Change the visibility of the scene debug cuboid.
/*!
 * Sets whether the scene debug cuboid is enabled (visible), and which illustrates the current view volume extent.
 */
void MildredWidget::setSceneCuboidEnabled(bool enabled)
{
    assert(sceneBoundingCuboidEntity_);
    sceneBoundingCuboidEntity_->setEnabled(enabled);
}
