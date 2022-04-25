#include "widget.h"
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

using namespace Mildred;

//! Create framegraph
/*!
 * Create a suitable framegraph - this will target an offscreen surface which we capture into a texture, and which is
 * subsequently rendered on "us" (a QOpenGLWidget). This is to overcome limitations with creating a Qt3D window and using Qt's
 * createWindowContainer().
 *
 * Much of this is taken direct from Florian Blume's qt3d-widget implementation at https://github.com/florianblume/qt3d-widget.
 *
 * The framegraph is constructed with the following structure:
 *
 *         QRenderStateSet          Root of the framegraph, sets up clipping, culling etc.
 *               |
 *      QRenderTargetSelector       Targets the offscreen colour and depth textures for rendering
 *               |
 *      QRenderSurfaceSelector      Sets the target surface to the offscreen surface
 *               |
 *           QViewport              Defines the viewport on the target surface
 *               |
 *         QClearBuffers            Sets the targets to clear and the default attributes to clear to
 *               |
 *        QCameraSelector           Selects an existing camera to view the scenegraph
 *               |
 *        QForwardRenderer          Basic forward renderer to execute the scenegraph
 */
void MildredWidget::createFrameGraph()
{
    // Set up our offscreen rendering surface
    auto samples = QSurfaceFormat::defaultFormat().samples();
    offscreenSurface_ = new QOffscreenSurface;
    offscreenSurface_->setFormat(QSurfaceFormat::defaultFormat());
    offscreenSurface_->create();

    // Create an aspect engine - we have to use the Automatic submission type as Manual seems to cause hangs.
    aspectEngine_ = new Qt3DCore::QAspectEngine;
    aspectEngine_->registerAspect(new Qt3DRender::QRenderAspect(Qt3DRender::QRenderAspect::Automatic));
    aspectEngine_->registerAspect(new Qt3DInput::QInputAspect);
    aspectEngine_->registerAspect(new Qt3DLogic::QLogicAspect);

    // Setup color
    colourOutput_ = new Qt3DRender::QRenderTargetOutput;
    colourOutput_->setAttachmentPoint(Qt3DRender::QRenderTargetOutput::Color0);

    // Create a color texture to render into
    colourTexture_ = new Qt3DRender::QTexture2D;
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
    depthTexture_ = new Qt3DRender::QTexture2D;
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

    /*
     * FrameGraph
     */

    // Create our render state set
    renderStateSet_ = new Qt3DRender::QRenderStateSet;
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
    camera_->lens()->setOrthographicProjection(0, width(), 0, height(), 0.1f, 1000.0f);
    camera_->setPosition(QVector3D(0, 0, 1.0f));
    camera_->setViewCenter(QVector3D(0, 0, -10.0));
    auto *cameraSelector = new Qt3DRender::QCameraSelector(clearBuffers);
    cameraSelector->setCamera(camera_);

    // Create a simple forward renderer
    auto *forwardRenderer_ = new Qt3DExtras::QForwardRenderer;
    forwardRenderer_->setCamera(camera_);
    forwardRenderer_->setSurface(offscreenSurface_);
    forwardRenderer_->setParent(camera_);

    // Finally, construct our render settings and point it to the top of the framegraph (our QRenderStateSet)
    renderSettings_ = new Qt3DRender::QRenderSettings;
    renderSettings_->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    renderSettings_->setActiveFrameGraph(renderStateSet_);
}
