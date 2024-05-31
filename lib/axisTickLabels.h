#pragma once

#include <QAbstractListModel>

class Axis;

class AxisTickLabels : public QAbstractListModel
{
  Q_OBJECT

 public:
  AxisTickLabels(Axis &parent);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

 private:
  Axis& parent_;
  const int N;
};
