#pragma once

#include <QVector3D>
#include <QQuick3DGeometry>

class LineGeometry : public QQuick3DGeometry
{
    Q_OBJECT
    QML_NAMED_ELEMENT(LineGeometry)
    Q_PROPERTY(float thickness MEMBER thickness_ NOTIFY dataChanged)
    Q_PROPERTY(QList<float> xs MEMBER xs_ NOTIFY dataChanged)
    Q_PROPERTY(QList<float> ys MEMBER ys_ NOTIFY dataChanged)

public:
    LineGeometry();
    void generateSines();

Q_SIGNALS:
  void dataChanged();

private:
    void updateData();
    float thickness_ = 0.01;
    QList<float> xs_, ys_;
};
