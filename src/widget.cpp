#include "widget.h"
#include "entities/axis.h"
#include "material.h"
#include <QOpenGLFunctions>
#include <QResizeEvent>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DInput/QInputAspect>
#include <Qt3DLogic/QLogicAspect>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>
#include <stdexcept>

// Initialise Qt resources
void initialiseQtResources()
{
    static auto initialised = false;
    if (!initialised)
        Q_INIT_RESOURCE(shaders);
    initialised = true;
}

using namespace Mildred;

//! Constructs a Mildred widget which is a child of \param parent.
MildredWidget::MildredWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    // Initialise resources
    initialiseQtResources();

    // Enable mouse tracking
    setMouseTracking(true);

    // Create our root entity
    rootEntity_ = Qt3DCore::QEntityPtr(new Qt3DCore::QEntity);

    // Create parameters
    sceneDataAxesParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxes"), QMatrix4x4());
    sceneDataAxesExtentsParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesExtents"), QVector3D());
    sceneDataAxesOriginParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesOrigin"), QVector3D());
    sceneDataTransformInverseParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataTransformInverse"), QMatrix4x4());
    viewportSizeParameter_ = new Qt3DRender::QParameter(QStringLiteral("viewportSize"), QVector2D());

    // Create framegraph
    createFrameGraph();

    // Set up basic scenegraph
    createSceneGraph();

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
        colourTexture_->setSize(width(), height());
        depthTexture_->setSize(width(), height());
        renderSurfaceSelector_->setExternalRenderTargetSize(QSize(width(), height()));

        updateMetrics();
        updateTransforms();
        updateShaderParameters();

        rootEntity_->addComponent(renderSettings_);
        aspectEngine_->setRootEntity(rootEntity_);
        initialised_ = true;

        // In order to reliably get an initial image on show, we need to update/process twice
        update();
        aspectEngine_->processFrame();
        update();
        aspectEngine_->processFrame();
    }

    QWidget::showEvent(e);
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
    glShaderProgram_->addShaderFromSourceCode(
        QOpenGLShader::Vertex,
        "#version 150\n"
        "in highp vec3 vertex;\n"
        "in mediump vec2 texCoord;\n"
        "out mediump vec2 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "        gl_Position = matrix * vec4(vertex, 1.0);\n"
        //                                                                     "        gl_Position = vec4(vertex, 1.0);\n"
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
    // Resizing the widget demands that the metrics information held in @class MildredMetrics is updated, ensuring the whole of
    // the available drawing surface is used for visualisation.
    updateMetrics();

    // Move the scene root position to be the centre of the XY plane and a suitable distance away
    sceneRootTransform_->setTranslation(QVector3D(w / 2.0, h / 2.0, -w));

    camera_->lens()->setOrthographicProjection(0, w, 0, h, 0.1f, width() * 2.0f);
    camera_->setAspectRatio(float(w) / float(h));

    // Resize our textures and the offscreen target
    colourTexture_->setSize(w, h);
    depthTexture_->setSize(w, h);
    renderSurfaceSelector_->setExternalRenderTargetSize(QSize(w, h));

    updateTransforms();
    updateShaderParameters();
}

void MildredWidget::paintGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width(), height());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, width(), 0.0, height(), -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShaderProgram_->bind();
    {
        QMatrix4x4 m;
        m.ortho(0, 1.0, 0., 1.0, 1.0f, 3.0f);
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
    auto *cuboidMaterial =
        createMaterial(sceneBoundingCuboidEntity_, RenderableMaterial::VertexShaderType::Unclipped,
                       RenderableMaterial::GeometryShaderType::None, RenderableMaterial::FragmentShaderType::Phong);
    cuboidMaterial->setAmbient(QColor(255, 0, 0, 100));
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
    xAxis_->setTitleText("X");
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
    yAxis_->setTitleText("Y");
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
    zAxis_->setTitleText("Z");
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
