import QtQuick
import QtQuick3D
import com.projectdissolve

Model {
    id: root
    property Axis axis

    geometry: root.axis 
    materials: [
        PrincipledMaterial {
            id: frame_material
            baseColor: "black"
            alphaMode: PrincipledMaterial.Opaque
        }
    ]
}
