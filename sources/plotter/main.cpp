#include <QtCore/QSettings>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQuickControls2/QQuickStyle>
#include <QtWidgets/QApplication>

#include <common/statistics.hpp>

#include "statistics_model.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("plotter_application");
  QApplication::setOrganizationName("Danil Sidoruk");

  QQmlApplicationEngine engine;
  auto context = engine.rootContext();

  StatisticsModel statisticsModel;

  Statistics control = Statistics::from("statistics.dump");
  auto stats = control.getStatistics();
  int i = 0;
  for (const auto &entry : control.getStatistics()) {
    StatisticsEntry point{.day = i};
    point.rabbit_count = entry.contains(ActorBase::State::RABBIT)
                             ? entry.at(ActorBase::State::RABBIT)
                             : 0;
    point.wolf_m_count = entry.contains(ActorBase::State::WOLF_M)
                             ? entry.at(ActorBase::State::WOLF_M)
                             : 0;
    point.wolf_f_count = entry.contains(ActorBase::State::WOLF_F)
                             ? entry.at(ActorBase::State::WOLF_F)
                             : 0;
    ++i;

    statisticsModel.addPoint(std::move(point));
  }

  engine.rootContext()->setContextProperty("statisticsModel", &statisticsModel);

  QObject::connect(&engine, &QQmlApplicationEngine::quit, &app,
                   &QGuiApplication::quit);
  engine.load(QUrl("qrc:/qt/qml/plotter_application/main.qml"));
  if (engine.rootObjects().isEmpty())
    return -1;

  return app.exec();
}