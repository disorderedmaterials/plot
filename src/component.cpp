#include "component.h"
#include "material.h"
#include <QResizeEvent>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QKeyboardHandler>
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
MildredWidget::MildredWidget()
{
    // Initialise resources
    initialiseQtResources();

    // TEST
    auto *material = new Qt3DExtras::QPhongMaterial;

    auto *line = new LineEntity(this);
    line->addVertices({{0.0, 0.0, 0.0}, {20.0, 0.0, 0.0}});
    line->setBasicIndices();
    line->finalise();
    line->addComponent(material);

    // Create parameters
    sceneDataAxesParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxes"), QMatrix4x4());
    sceneDataAxesExtentsParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesExtents"), QVector3D());
    sceneDataAxesOriginParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataAxesOrigin"), QVector3D());
    sceneDataTransformInverseParameter_ = new Qt3DRender::QParameter(QStringLiteral("sceneDataTransformInverse"), QMatrix4x4());
    viewportSizeParameter_ = new Qt3DRender::QParameter(QStringLiteral("viewportSize"), QVector2D());

    // Add a mouse handler and connect it up
    auto *mouseHandler = new Qt3DInput::QMouseHandler(this);
    auto *mouseDevice = new Qt3DInput::QMouseDevice(this);
    mouseHandler->setSourceDevice(mouseDevice);
    addComponent(mouseHandler);
    connect(mouseHandler, SIGNAL(positionChanged(Qt3DInput::QMouseEvent *)), this,
            SLOT(mousePositionChanged(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(pressed(Qt3DInput::QMouseEvent *)), this, SLOT(mouseButtonPressed(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(released(Qt3DInput::QMouseEvent *)), this,
            SLOT(mouseButtonReleased(Qt3DInput::QMouseEvent *)));
    connect(mouseHandler, SIGNAL(wheel(Qt3DInput::QWheelEvent *)), this, SLOT(mouseWheeled(Qt3DInput::QWheelEvent *)));

    // Set up basic scenegraph
    createSceneGraph();

    // Connect the metrics object and update
    connect(&metrics_, SIGNAL(metricsChanged()), this, SLOT(updateTransforms()));

    updateMetrics();
}

/*
 * Metrics
 */

//! Set viewport size
/*!
 * Resizing the viewport demands that the metrics information held in @class MildredMetrics is updated, ensuring the whole of
 * the available drawing surface is used for visualisation.
 */
void MildredWidget::setViewportSize(int w, int h)
{
    viewportWidth_ = w;
    viewportHeight_ = h;

    updateMetrics();

    // Move the scene root position to be the centre of the XY plane and a suitable distance away along -Z
    sceneRootTransform_->setTranslation(QVector3D(viewportWidth_ / 2.0, viewportHeight_ / 2.0, -viewportWidth_ / 2.0));

    // Debug objects
    sceneBoundingCuboidTransform_->setScale3D(QVector3D(viewportWidth_, viewportHeight_, viewportWidth_));

    // Update parameters and transforms
    updateTransforms();
    //    updateShaderParameters();
}

//! Update metrics for current surface size
/*!
 *  Updates the internal @class MildredMetrics object.
 */
void MildredWidget::updateMetrics() { metrics_.update(viewportWidth_, viewportHeight_, xAxis_, yAxis_); }

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
 * Materials
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

/*
 * Display Data
 */

//! Set the target display data
void MildredWidget::setData(DataSource *data)
{
    // Check for existing data
    if (data_)
    {
        // TODO Disconnect signals from object before we stop using it
    }

    data_ = data;

    // Connect up signals
    if (data_)
    {
        // TODO
        //     connect(&metrics_, SIGNAL(metricsChanged()), this, SLOT(updateTransforms()));
    }
}

/*
 * Display Entities
 */

// Add new 1-dimensional data entity for supplied data
Data1DEntity *MildredWidget::createData1DEntity(std::string_view tag)
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

    // Add a material
    auto *material = createMaterial(entity, RenderableMaterial::VertexShaderType::ClippedToDataVolume,
                                    RenderableMaterial::GeometryShaderType::LineTesselator,
                                    RenderableMaterial::FragmentShaderType::PerVertexPhong);
    entity->setDataMaterial(material);
    entity->setErrorMaterial(material);
    entity->setSymbolMaterial(material);

    return entity;
}
