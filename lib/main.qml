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

        Node {
            position: Qt.vector3d(0, 0, 120)
            Item {
                width: 400
                height: 400
                anchors.centerIn: parent

                Text {
                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 20
                    color: "#00e0e0"
                    text: qsTr("Background Item")
                }
            }
        }

         DirectionalLight {
            ambientColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
            brightness: 1.0
            eulerRotation.x: -25
        }

        LineModel {
            id: plotLine
            thickness: thickness.value
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

        Column {
            Label {text: "Thickness"}
            Slider {
                id: thickness
                from: 0.001
                to: 0.1
            }
            Label {text: "Count"}
            Slider {
                id: count
                from: 10
                to: 1000
                value: 20
            }
            Label {text: "Amplitude"}
            Slider {
                id: amplitude
                from: 0
                to: 5
                value: 1
            }
            Label {text: "Frequency"}
            Slider {
                id: frequency
                from: 0
                to: 10
                value: 3
                onMoved: renderButton.onClicked()
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
