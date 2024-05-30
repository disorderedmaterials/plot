#pragma once

#include <QVector3D>
#include <QQuick3DGeometry>

class Axis : public QQuick3DGeometry
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Axis)
    Q_PROPERTY(double thickness MEMBER thickness_ NOTIFY dataChanged)
    Q_PROPERTY(double minimum MEMBER minimum_ NOTIFY dataChanged)
    Q_PROPERTY(double maximum MEMBER maximum_ NOTIFY dataChanged)
    Q_PROPERTY(bool direction MEMBER direction_ NOTIFY dataChanged)

public:
    Axis();

Q_SIGNALS:
  void dataChanged();

private:
    void updateData();
    bool direction_;
    double minimum_, maximum_, thickness_;
};
