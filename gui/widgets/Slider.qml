import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property alias inverted: model.inverted
    property alias value: model.value;
    property real knobSize: height * 0.2;

    property color knobColor: "black"
    property color backgroundColor: "grey"
    property color borderColor: "black"
    property Component knob:
        Rectangle{ color: knobColor }
    property Component background:
        Rectangle { color: backgroundColor; border.color: borderColor }

    RangeModel { id: model }

    Loader {
        property alias model: model
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
        onPressed: setValue(mouse)
        onPositionChanged: setValue(mouse)
        function setValue(mouse) {
            model.position = (orientation == Qt.Vertical) ? mouse.y : mouse.x;
        }
    }

    StateGroup {
        states: [
            State {
                when: orientation == Qt.Vertical
                PropertyChanges {
                    target: model
                    minimumPosition: mouseArea.y + mouseArea.height - (knobSize / 2)
                    maximumPosition: mouseArea.y + (knobSize / 2)
                }
                PropertyChanges {
                    target: knobItem
                    height: knobSize
                    y: model.position - (knobSize / 2);
                }
                AnchorChanges {
                    target: knobItem
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
            },
            State {
                when: orientation == Qt.Horizontal
                PropertyChanges {
                    target: model
                    minimumPosition: mouseArea.x + (knobSize / 2)
                    maximumPosition: mouseArea.x + mouseArea.width - (knobSize / 2)
                }
                PropertyChanges {
                    target: knobItem
                    width: knobSize
                    x: model.position - (knobSize / 2);
                }
                AnchorChanges {
                    target: knobItem
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                }
            }
        ]
    }
}
