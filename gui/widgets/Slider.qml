import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property var margins:
        (orientation == Qt.Vertical) ?
            [knobItem.height / 2, knobItem.height / 2] :
            [knobItem.width / 2, knobItem.width / 2]
    property alias value: model.value;

    property Component knob:
        Rectangle{ width:15; height:15; color: knobColor }
    property Component background:
        Rectangle { color: backgroundColor; border.color: borderColor }
    property color knobColor: "black"
    property color backgroundColor: "grey"
    property color borderColor: "black"

    RangeModel { id: model }

    Loader {
        sourceComponent: background
        anchors.fill: parent
    }

    Loader {
        id: knobItem
        sourceComponent: knob
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: updateValue(mouse)
        onPositionChanged: updateValue(mouse)
        function updateValue(mouse) {
            var range = ((orientation == Qt.Vertical) ? height : width) - margins[0] - margins[1];
            var pos = (orientation == Qt.Vertical) ? mouse.y : mouse.x;
            slider.value = (pos - margins[0]) / range;
        }
    }

    states: [
        State {
            when: orientation == Qt.Vertical
            PropertyChanges {
                target: knobItem
                anchors.verticalCenter: undefined
                anchors.horizontalCenter: parent.horizontalCenter
                y: Math.floor (
                       value * (slider.height - margins[0] - margins[1])
                       + margins[0] - (height/2) )
            }
            /*PropertyChanges {
                target: mouseArea
                onPressed: { slider.value = mouse.y / height }
                onPositionChanged: { slider.value = mouse.y / height }
            }*/
        },
        State {
            when: orientation == Qt.Horizontal
            PropertyChanges {
                target: knobItem
                anchors.horizontalCenter: undefined
                anchors.verticalCenter: parent.verticalCenter
                x: Math.floor(
                       value * (slider.width - margins[0] - margins[1])
                       + margins[0] - (width/2) );
            }
            /*PropertyChanges {
                target: mouseArea
                onPressed: { slider.value = mouse.x / width }
                onPositionChanged: { slider.value = mouse.x / width }
            }*/
        }
    ]
}
