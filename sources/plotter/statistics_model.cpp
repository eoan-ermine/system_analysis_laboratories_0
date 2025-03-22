#include "statistics_model.hpp"

StatisticsModel::StatisticsModel(QObject *parent)
    : QAbstractListModel(parent) {}

int StatisticsModel::rowCount(const QModelIndex &) const {
  return m_points.size();
}

QVariant StatisticsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_points.size())
    return QVariant();

  const StatisticsEntry &point = m_points.at(index.row());
  if (role == DAY_ROLE)
    return point.day;
  if (role == RABBIT_ROLE)
    return point.rabbit_count;
  if (role == WOLF_M_ROLE)
    return point.wolf_m_count;
  if (role == WOLF_Y_ROLE)
    return point.wolf_f_count;

  return QVariant();
}

QHash<int, QByteArray> StatisticsModel::roleNames() const {
  return {{DAY_ROLE, "day"},
          {RABBIT_ROLE, "rabbit"},
          {WOLF_M_ROLE, "male_wolf"},
          {WOLF_Y_ROLE, "female_wolf"}};
}

void StatisticsModel::addPoint(StatisticsEntry entry) {
  beginInsertRows(QModelIndex(), m_points.size(), m_points.size());
  m_points.push_back(std::move(entry));
  endInsertRows();
}

void StatisticsModel::clearPoints() {
  beginResetModel();
  m_points.clear();
  endResetModel();
}
