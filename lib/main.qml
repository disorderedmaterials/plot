import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Controls
import com.projectdissolve

ApplicationWindow {
    id: root
    property vector3d scale: Qt.vector3d(Math.min(graphView.width/2.5, graphView.height/2.5), Math.min(graphView.width/2.5, graphView.height/2.5), 200)

    height: 600
    title: "Mildred QML Test"
    visible: true
    width: 800

    Node {
         id: standAloneScene

         DirectionalLight {
            ambientColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
            brightness: 1.0
            eulerRotation.x: -25
        }

        LineModel {
            id: plotLine
            thickness: thickness.value
            color: colorDialog.selectedColor
            scale: root.scale
            xAxis: xAxis
            yAxis: yAxis
        }

        LineModel {
            thickness: thickness.value
            color: "#006f6f"
            xs: [-1.0, -0.8, -0.6, -0.4, -0.2, 0, 0.2, 0.4, 0.6, 0.8, 1.0]
            ys: [1.0, 0.64, 0.36, 0.16, 0.04, 0, 0.04, 0.16, 0.36, 0.64, 1.0]
            scale: root.scale
            xAxis: xAxis
            yAxis: yAxis
        }

        AxisModel {
            axis: Axis {
                id: xAxis
                thickness: 0.01
                direction: false
                minimum: -1.0
                maximum: 1.0
            }
            scl: root.scale
            color: "black"
        }

        AxisModel {
            axis: Axis {
                id: yAxis
                thickness: 0.01
                direction: true;
                minimum: -1.0
                maximum: 1.0
            }
            scl: root.scale
            color: "black"
        }
    }


    View3D {
        id: graphView
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: settingsPane.left

        camera: OrthographicCamera {
            id: cameraOrthographicLeft
            z: 600
        }
        importScene: standAloneScene

        OrbitCameraController {
            anchors.fill: parent
            origin: standAloneScene
            camera: cameraOrthographicLeft
        }
    }
    Pane {
        id: settingsPane
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 200

        Column {
            Label {text: "Thickness"}
            Slider {
                id: thickness
                from: 0.01
                to: 0.1
            }
            Label {text: "Count"}
            Slider {
                id: count
                from: 10
                to: 1000
                value: 20
                onMoved: renderButton.onClicked()
            }
            Label {text: "Amplitude"}
            Slider {
                id: amplitude
                from: 0
                to: 5
                value: 1
                onMoved: renderButton.onClicked()
            }
            Label {text: "Frequency"}
            Slider {
                id: frequency
                from: 0
                to: 10
                value: 3
                onMoved: renderButton.onClicked()
            }
            ColorDialog {
                id: colorDialog
                selectedColor: "#ff00ff"
            }
            Button {
                text: "Color"
                onClicked: colorDialog.open()
            }
            Button {
                id: renderButton
                text: "render"
                onClicked: {
                    var xs = []
                    var ys = []
                    for (var i=0; i<count.value; i++) {
                        var x = -1.0 + 2 * i/count.value
                        xs.push(x)
                        ys.push(amplitude.value * Math.sin(frequency.value * Math.PI * x));
                    }
                    plotLine.xs = xs;
                    plotLine.ys = ys;
                }
            }
        }
    }
}
