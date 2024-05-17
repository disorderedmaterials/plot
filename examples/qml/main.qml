import QtQuick 2.0
import QtQuick.Controls 2.0
import Qt3D.Render
import Qt3D.Core
import Qt3D.Extras
import Qt3D.Logic
import QtQuick3D
import com.projectdissolve

ApplicationWindow {
    height: 600
    title: "Mildred QML Test"
    visible: true
    width: 800
    signal resized

    View3D {
        id: scene3D
        signal resized
        anchors.fill: parent
        anchors.margins: 10
        focus: true

        environment: SceneEnvironment {
            clearColor: "white"
            backgroundMode: SceneEnvironment.Color
        }
        // Creating a Camera like this works perfectly for Scene3D, but not for View3D
        // camera: Camera {   // error: "Camera is Abstract"
        //     id: camera
        //     farPlane: 1000.0
        //     fieldOfView: 45
        //     nearPlane: 0.1
        //     position: Qt.vector3d(0.0, 0.0, 1.0)
        //     projectionType: CameraLens.OrthographicProjection
        //     upVector: Qt.vector3d(0.0, 1.0, 0.0)
        //     viewCenter: Qt.vector3d(0.0, 0.0, -1.0)

        camera: OrthographicCamera {
            position: Qt.vector3d(0.0, 100.0, 300.0)
            eulerRotation.x: -30
        }

        DirectionalLight {
            eulerRotation.x: -30
            eulerRotation.y: -70
        }
        RenderStateSet {
            renderStates: [
                ClipPlane {
                    distance: 0.1
                    enabled: true
                    normal: Qt.vector3d(1.0, 0.0, 0.0)
                    planeIndex: 0
                },
                ClipPlane {
                    distance: 1.0
                    enabled: true
                    normal: Qt.vector3d(-1.0, 0.0, 0.0)
                    planeIndex: 1
                },
                ClipPlane {
                    distance: 0.0
                    enabled: true
                    normal: Qt.vector3d(0.0, 1.0, 0.0)
                    planeIndex: 2
                },
                ClipPlane {
                    distance: 0.0
                    enabled: true
                    normal: Qt.vector3d(0.0, -1.0, 0.0)
                    planeIndex: 3
                },
                ClipPlane {
                    distance: 0.0
                    enabled: true
                    normal: Qt.vector3d(0.0, 0.0, 1.0)
                    planeIndex: 4
                },
                ClipPlane {
                    distance: 0.0
                    enabled: true
                    normal: Qt.vector3d(0.0, 0.0, -1.0)
                    planeIndex: 5
                }
            ]
        }

        Model {
            position: Qt.vector3d(0, -200, -100)
            source: "#Cylinder"
            scale: Qt.vector3d(2, 0.2, 1)
            materials: [DefaultMaterial {
                diffuseColor: "red"
            }
            ]

            // Not visible - because this is a QEntity?
            MildredWidget {
                id: visualiser
                sourceData: mildredData ? mildredData : null
            }
        }
    }
}
