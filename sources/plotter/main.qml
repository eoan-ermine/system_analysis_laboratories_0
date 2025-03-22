import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15

ApplicationWindow {
    visible: true
    width: 600
    height: 400
    title: "Plotter"

    Column {
        anchors.fill: parent
        spacing: 10

        ChartView {
            id: chart
            width: parent.width
            height: 300
            antialiasing: true

            ValueAxis {
                id: axisX
            }

            ValueAxis {
                id: axisY
            }

            LineSeries {
                id: rabbitSeries
                name: "Rabbits"
                axisX: axisX
                axisY: axisY

                Component.onCompleted: {
                    for (var i = 0; i < statisticsModel.rowCount(); i++) {
                        append(statisticsModel.data(statisticsModel.index(i, 0), 1), statisticsModel.data(statisticsModel.index(i, 0), 2));
                    }
                    updateAxes()
                }
            }

            LineSeries {
                id: maleWolvesSeries
                name: "Male wolves"
                axisX: axisX
                axisY: axisY

                Component.onCompleted: {
                    for (var i = 0; i < statisticsModel.rowCount(); i++) {
                        append(statisticsModel.data(statisticsModel.index(i, 0), 1), statisticsModel.data(statisticsModel.index(i, 0), 3));
                    }
                    updateAxes()
                }
            }

            LineSeries {
                id: femaleWolvesSeries
                name: "Female wolves"
                axisX: axisX
                axisY: axisY

                Component.onCompleted: {
                    for (var i = 0; i < statisticsModel.rowCount(); i++) {
                        append(statisticsModel.data(statisticsModel.index(i, 0), 1), statisticsModel.data(statisticsModel.index(i, 0), 4));
                    }
                    updateAxes()
                }
            }
        }
    }

    function updateAxes() {
        var minX = Number.MAX_VALUE, maxX = Number.MIN_VALUE
        var minY = Number.MAX_VALUE, maxY = Number.MIN_VALUE

        function processSeries(series) {
            for (var i = 0; i < series.count; i++) {
                var point = series.at(i)
                minX = Math.min(minX, point.x)
                maxX = Math.max(maxX, point.x)
                minY = Math.min(minY, point.y)
                maxY = Math.max(maxY, point.y)
            }
        }

        processSeries(rabbitSeries)
        processSeries(maleWolvesSeries)
        processSeries(femaleWolvesSeries)

        axisX.min = minX
        axisX.max = maxX
        axisY.min = minY
        axisY.max = maxY
    }
}
