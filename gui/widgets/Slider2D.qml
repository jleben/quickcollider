import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property var margins: {
        var mx = knobItem.width / 2;
        var my = knobItem.height / 2;
        [mx, my, mx, my]
    }

    property alias xValue: xModel.value
    property alias yValue: yModel.value

    property Component knob:
        Rectangle { width:15; height:15; color: knobColor }
    property Component background:
        Rectangle { color: backgroundColor; border.color: borderColor }
    property Component border
    property color knobColor: "black"
    property color backgroundColor: "grey"
    property color borderColor: "black"


    RangeModel { id: xModel }
    RangeModel { id: yModel }

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
        x: Math.floor( xValue * (slider.width - margins[0] - margins[2]) + margins[0] - (width / 2) );
        y: Math.floor( yValue * (slider.height - margins[1] - margins[3]) + margins[1] - (height / 2) );
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: updateValue(mouse)
        onPositionChanged: updateValue(mouse)
        function updateValue(mouse) {
            var xRange = width - margins[0] - margins[2];
            xValue = (mouse.x - margins[0]) / xRange;
            var yRange = height - margins[1] - margins[3];
            yValue = (mouse.y - margins[1]) / yRange;
        }
    }
}
