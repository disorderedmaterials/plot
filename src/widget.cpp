#include "widget.h"
#include "material.h"
#include <QResizeEvent>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
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
MildredWidget::MildredWidget(QWidget *parent) : QWidget(parent)
{
    // Initialise resources
    initialiseQtResources();

    /*
     * In order to get a suitable surface to draw on we must first create a full Qt3DWindow and then capture it in
     * a container widget.
     */
    viewWindow_ = new Qt3DExtras::Qt3DWindow();

    // Create a container for the Qt3DWindow
    viewContainer_ = createWindowContainer(viewWindow_, this);

    // Create our root entity
    rootEntity_ = Qt3DCore::QEntityPtr(new Qt3DCore::QEntity);

    // Create a QRenderSettings and add it as a component to the root entity
    renderSettings_ = viewWindow_->renderSettings();

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

    // Construct a camera
    camera_ = new Qt3DRender::QCamera(rootEntity_.data());
    //    camera_->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setPosition(QVector3D(0, 0, 1.0f));
    camera_->setViewCenter(QVector3D(0, 0, -10.0));

    // Create the framegraph
    frameGraph_.create(renderSettings_, viewWindow_, camera_);

    // Set up basic scenegraph
    createSceneGraph();

    // Set the main root entity
    viewWindow_->setRootEntity(rootEntity_.data());

    // Connect the metrics object and update
    connect(&metrics_, SIGNAL(metricsChanged()), this, SLOT(updateTransforms()));
}

/*
 * QWidget
 */

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

    // Update parameters and transforms
    updateTransforms();
    updateShaderParameters();

    // Lastly, resize our view container
    viewContainer_->resize(this->size());
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
                                    RenderableMaterial::GeometryShaderType::LineTesselator,
                                    RenderableMaterial::FragmentShaderType::PerVertexPhong);
    material->setAmbient(Qt::blue);
    material->addParameter(new Qt3DRender::QParameter(QStringLiteral("lineWidth"), 1.5f));
    entity->setDataMaterial(material);

    return entity;
}

/*
 * Grouping
 */

// Create new display group
DisplayGroup *MildredWidget::addDisplayGroup()
{
    auto newGroup = displayGroups_.emplace_back(std::make_shared<DisplayGroup>());

    return newGroup.get();
}