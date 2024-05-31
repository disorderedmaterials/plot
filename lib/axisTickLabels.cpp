#include "axisTickLabels.h"
#include "axis.h"
#include <iostream>

AxisTickLabels::AxisTickLabels(Axis &parent) : parent_(parent), N(5) {
}

int AxisTickLabels::rowCount(const QModelIndex &parent) const {
  return N;
}

QVariant AxisTickLabels::data(const QModelIndex &index, int role) const {
  switch (role) {
  case Qt::UserRole: return QString("%1").arg(parent_.minimum() + index.row()/((double) N - 1) * (parent_.maximum() - parent_.minimum()));
  case (Qt::UserRole + 1): return index.row();
  case (Qt::UserRole + 2): return index.row();
  default: return index.row();
  }
}

QHash<int, QByteArray> AxisTickLabels::roleNames() const {
  std::cout << "Role Names" << std::endl;
  QHash<int, QByteArray> roles;
  roles[Qt::UserRole] = "tickLabel";
  roles[Qt::UserRole + 1] = "tickX";
  roles[Qt::UserRole + 2] = "tickY";
  return roles;
}
