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

        entity: Entity {
            id: root3DEntity
            RenderSettings {
                id: renderSettings

                activeFrameGraph: ForwardRenderer {
                    id: renderer
                    clearColor: "#ffff4f"

                    camera: Camera {
                        id: camera
                        farPlane: 1000.0
                        fieldOfView: 45
                        nearPlane: 0.1
                        position: Qt.vector3d(0.0, 0.0, 100.0)
                        projectionType: CameraLens.PerspectiveProjection
                        upVector: Qt.vector3d(0.0, 1.0, 0.0)
                        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
                    }
                }
            }
            MildredWidget {
                id: myRootEntity
            }
        }
    }
}
