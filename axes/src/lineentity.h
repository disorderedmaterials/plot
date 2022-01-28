#include <Qt3DCore/QEntity>
#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

class LineGeometry : public Qt3DCore::QEntity
{
public:
    LineGeometry(Qt3DCore::QNode *parent = nullptr);
    ~LineGeometry() = default;

    public:
    Qt3DCore::QGeometry geometry_;
    Qt3DRender::QGeometryRenderer geometryRenderer_;
};
