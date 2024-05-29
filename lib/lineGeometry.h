#pragma once

#include <QVector3D>
#include <QQuick3DGeometry>

class LineGeometry : public QQuick3DGeometry
{
    Q_OBJECT
    QML_NAMED_ELEMENT(LineGeometry)
    Q_PROPERTY(float thickness MEMBER thickness_ NOTIFY changeGeometry)

public:
    LineGeometry();
    void changeGeometry();

private:
    void updateData();
    float thickness_ = 0.01;
};
