#pragma once

#include <QVector3D>
#include <QQuick3DGeometry>

class LineGeometry : public QQuick3DGeometry
{
    Q_OBJECT
    QML_NAMED_ELEMENT(LineGeometry)

public:
    LineGeometry();

private:
    void updateData();

};
