import QtQuick 2.0
import QuickCollider 0.1

Item {
    property string file
    property alias spacing: waveform.spacing
    property color backgroundColor: Qt.rgba(0.1, 0.1, 0.1)
    property Component background: defaultBackground
    property real mouseZoomFactor: 0.01
    property real wheelZoomFactor: 0.001

    onFileChanged: { if (file.length) waveform.load(file) }

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

    MouseArea {
        anchors.fill: parent
        property point previousPos
        onPressed: {
            previousPos = Qt.point(mouse.x, mouse.y);
        }
        onPositionChanged: {
            var frameDif = (mouse.x - previousPos.x) * waveform.framesPerPixel;
            waveform.firstVisibleFrame = waveform.firstVisibleFrame - frameDif;
            if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.y != previousPos.y))
            {
                var zoom = waveform.zoom * Math.pow(2, (previousPos.y - mouse.y) * mouseZoomFactor);
                waveform.zoomTo( mouse.x, zoom );
            }
            previousPos = Qt.point(mouse.x, mouse.y);
        }
        onWheel: {
            var rotation = wheel.angleDelta.y;
            var zoom = waveform.zoom * Math.pow(2, rotation * wheelZoomFactor);
            waveform.zoomTo( wheel.x, zoom );
        }
    }
}
