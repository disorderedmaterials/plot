import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt3D.Render
import Qt3D.Core
import Qt3D.Extras
import Qt3D.Logic
import QtQuick.Scene3D
import com.projectdissolve

ApplicationWindow {
    height: 600
    title: "Mildred QML Test"
    visible: true
    width: 800

    Scene3D {
        id: scene3D
        anchors.fill: parent
        anchors.margins: 10
        focus: true
        signal resized()
        onWidthChanged: resized()
        onHeightChanged: resized()
        onResized: () =>  { visualiser.setViewportSize(width, height);
            camera.lens.left = 0;
            camera.lens.right = width;
            camera.lens.top = height;
            camera.lens.bottom = 0;
            camera.lens.near = 0.1;
            camera.lens.far = width * 2;
        }

        entity: Entity {
            id: root3DEntity
            RenderSettings {
                id: renderSettings

                activeFrameGraph: ForwardRenderer {
                    id: renderer
                    clearColor: "#ffffff"

                    camera: Camera {
                        id: camera
                        farPlane: 1000.0
                        fieldOfView: 45
                        nearPlane: 0.1
                        position: Qt.vector3d(0.0, 0.0, 1.0)
                        projectionType: CameraLens.OrthographicProjection
                        upVector: Qt.vector3d(0.0, 1.0, 0.0)
                        viewCenter: Qt.vector3d(0.0, 0.0, -1.0)
                    }
                }
            }
            ClipPlane {
                planeIndex: 0
                normal: Qt.vector3d(0.0, 1.0, 0.0)
            }
            ClipPlane {
                planeIndex: 1
                normal: Qt.vector3d(0.0, 1.0, 0.0)
            }
            ClipPlane {
                planeIndex: 2
                normal: Qt.vector3d(0.0, 1.0, 0.0)
            }
            ClipPlane {
                planeIndex: 3
                normal: Qt.vector3d(0.0, 1.0, 0.0)
            }
            ClipPlane {
                normal: Qt.vector3d(0.0, 1.0, 0.0)
                planeIndex: 4
            }
            ClipPlane {
                normal: Qt.vector3d(0.0, 1.0, 0.0)
                planeIndex: 5
            }
            MildredWidget {
                id: visualiser
                sourceData: mildredData ? mildredData : null
            }
        }
    }


}
