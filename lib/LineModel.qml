import QtQuick3D
import com.projectdissolve

Model {
    id: root
    property double thickness
    property list<double> xs
    property list<double> ys
    geometry: LineGeometry {
        id: plotLine
        thickness: root.thickness
        xs: root.xs
        ys: root.ys
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
