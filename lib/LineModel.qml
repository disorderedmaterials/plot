import QtQuick3D
import com.projectdissolve

Model {
    id: root
    property double thickness
    /* source: "#Cube" */
    geometry: LineGeometry {
        id: plotLine
        thickness: root.thickness
    }
    scale: Qt.vector3d(200, 200, 200)
    materials: [
        PrincipledMaterial {
            id: frame_material
            baseColor: "#ff00ff"
            alphaMode: PrincipledMaterial.Opaque
        }
    ]
}
