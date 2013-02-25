import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property alias xValue: xModel.value
    property alias yValue: yModel.value
    property alias xInverted: xModel.inverted
    property alias yInverted: yModel.inverted
    property real knobWidth: width * 0.2
    property real knobHeight: height * 0.2
    property color knobColor: "black"
    property color backgroundColor: "grey"
    property color borderColor: "black"

    property Component knob:
        Rectangle { color: knobColor }
    property Component background:
        Rectangle { color: backgroundColor; border.color: borderColor }
    property Component border


    onXValueChanged: { console.log(xValue) }

    RangeModel {
        id: xModel;
        minimumPosition: mouseArea.x + (knobWidth / 2)
        maximumPosition: mouseArea.x + mouseArea.width - (knobWidth / 2)
    }
    RangeModel {
        id: yModel
        minimumPosition: mouseArea.y + mouseArea.height - (knobHeight / 2)
        maximumPosition: mouseArea.y + (knobHeight / 2)
    }

    Loader {
        sourceComponent: background
        anchors.fill: parent
    }
    Loader {
        id: bkgLoader
        sourceComponent: border
        anchors.fill: parent
    }
    Loader {
        id: knobItem
        sourceComponent: knob
        width: knobWidth
        height: knobHeight
        x: xModel.position - (knobWidth / 2)
        y: yModel.position - (knobHeight / 2)
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: updateValue(mouse)
        onPositionChanged: updateValue(mouse)
        function updateValue(mouse) {
            xModel.position = mouse.x;
            yModel.position = mouse.y;
        }
    }
}
