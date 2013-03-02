import QtQuick 2.0
import QuickCollider 0.1

Item {
    property alias spacing: waveform.spacing
    property color backgroundColor: "black"
    property Component background: defaultBackground

    function load (filename) { waveform.load(filename) }

    Component {
        id: defaultBackground
        Rectangle { color: backgroundColor }
    }

    Loader {
        sourceComponent: background
        anchors.fill: parent
    }

    WaveformPlotter {
        id: waveform
        anchors.fill: parent
    }
}
