import QtQuick 2.0
import QuickCollider 0.1

Item {
    property alias server: plotter.server // server port
    property alias buffer: plotter.buffer // buffer index
    property alias running: plotter.running
    property alias updateInterval: plotter.updateInterval
    property alias mode: plotter.mode
    property alias xZoom: plotter.xZoom
    property alias yZoom: plotter.yZoom
    property color backgroundColor: Qt.rgba(0.1,0.1,0.1)
    property color borderColor: "black"
    property alias plotColors: plotter.trackColors

    function start() { plotter.start() }
    function stop() { plotter.stop() }

    Rectangle {
        id: bkgItem
        anchors.fill: parent
        color: backgroundColor
        border.color: borderColor
        border.width: 1

        OscilloscopePlotter
        {
            id: plotter
            anchors.fill: parent
            anchors.margins: 1
            clip: true
        }
    }
}
