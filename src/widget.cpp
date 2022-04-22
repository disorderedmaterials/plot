#include "widget.h"
#include "entities/axis.h"
#include "material.h"
#include <QOpenGLFunctions>
#include <QResizeEvent>
#include <Qt3DCore/QAspectEngine>
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
    glShaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders/texture.vert");
    glShaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shaders/texture.frag");
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
        printf("Data with tag '%s' already exists, so can't add it again.\n", it->first.c_str());
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
