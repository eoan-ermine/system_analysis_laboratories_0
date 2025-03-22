#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <vector>

struct StatisticsEntry {
  int day{0};
  int rabbit_count{0};
  int wolf_m_count{0};
  int wolf_f_count{0};
};

class StatisticsModel : public QAbstractListModel {
  Q_OBJECT
public:
  explicit StatisticsModel(QObject *parent = nullptr);

  enum PointRoles {
    DAY_ROLE = 1,
    RABBIT_ROLE = 2,
    WOLF_M_ROLE = 3,
    WOLF_Y_ROLE = 4
  };
  Q_ENUM(PointRoles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void addPoint(StatisticsEntry);
  Q_INVOKABLE void clearPoints();

private:
  std::vector<StatisticsEntry> m_points;
};
