#pragma once

#include <QVector3D>
#include <QQuick3DGeometry>

class LineGeometry : public QQuick3DGeometry
{
    Q_OBJECT
    QML_NAMED_ELEMENT(LineGeometry)
    Q_PROPERTY(double thickness MEMBER thickness_ NOTIFY dataChanged)
    Q_PROPERTY(QList<double> xs MEMBER xs_ NOTIFY dataChanged)
    Q_PROPERTY(QList<double> ys MEMBER ys_ NOTIFY dataChanged)

public:
    LineGeometry();
    void generateSines();

Q_SIGNALS:
  void dataChanged();

private:
    void updateData();
    double thickness_ = 0.01;
    QList<double> xs_, ys_;
};
