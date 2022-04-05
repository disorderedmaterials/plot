#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QLineWidth>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>

#include "framegraph.h"

//! Create the framegraph
/*!
 * Create a suitable framegraph for the supplied QRenderSettings @param parent and specified @param surface.
 * @param camera
 *
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
void MildredFrameGraph::create(Qt3DRender::QRenderSettings *parent, Qt3DExtras::Qt3DWindow *surface,
                               Qt3DRender::QCamera *camera)
{
    // Set our rendering surface
    auto *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector(parent);
    surfaceSelector->setSurface(surface);

    // Define a viewport to cover the entire surface
    auto *viewport = new Qt3DRender::QViewport(surfaceSelector);
    viewport->setNormalizedRect({0.0, 0.0, 1.0, 1.0});

    // Clear to background colour
    auto *clearBuffers = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(QColor(255, 255, 255, 255));

    // Set the camera to the one which was passed in
    auto *cameraSelector = new Qt3DRender::QCameraSelector(clearBuffers);
    cameraSelector->setCamera(camera);

    // Create a render state set
    auto *renderStateSet = new Qt3DRender::QRenderStateSet(cameraSelector);
    // -- Enable six clip planes for the data viewing volume
    for (auto n=0; n<6; ++n)
    {
        auto *cp = new Qt3DRender::QClipPlane;
        cp->setPlaneIndex(n);
        cp->setEnabled(true);
        renderStateSet->addRenderState(cp);
    }
    auto *cull = new Qt3DRender::QCullFace();
    cull->setMode(Qt3DRender::QCullFace::NoCulling);
    renderStateSet->addRenderState(cull);

    // Finally, set the active framegraph of the QRenderSettings to the top node of our framegraph
    parent->setActiveFrameGraph(surfaceSelector);
}
