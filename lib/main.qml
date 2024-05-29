import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick3D
import QtQuick.Controls
import com.projectdissolve

ApplicationWindow {
    height: 600
    title: "Mildred QML Test"
    visible: true
    width: 800

    LineGeometry {}

    Node {
         id: standAloneScene
         DirectionalLight {
            ambientColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
            brightness: 1.0
            eulerRotation.x: -25
        }
        Model {
            /* source: "#Cube" */
            geometry: LineGeometry {
                thickness: thickness.value
            }
            scale: Qt.vector3d(200, 200, 200)
            materials: [
                PrincipledMaterial {
                    id: frame_material
                    baseColor: "#ff0000"
                    alphaMode: PrincipledMaterial.Opaque
                }
            ]
        }
    }

    View3D {
        id: graphView
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: settingsPane.left

        camera: PerspectiveCamera {
            id: cameraOrthographicLeft
            z: 600
        }
        importScene: standAloneScene
    }
    Pane {
        id: settingsPane
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 200
        Label {
            text: "Thickness"
        }
        Slider {
            id: thickness
            from: 0
            to: 0.1
        }
    }
}
