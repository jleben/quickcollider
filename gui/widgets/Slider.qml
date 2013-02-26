import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property alias inverted: model.inverted
    property alias steps: model.steps
    property alias value: model.value;
    property real knobSize: 15;

    property color knobColor: Qt.rgba(0.1,0.1,0.1)
    property color backgroundColor: "grey"
    property color borderColor: "black"

    property Component background: defaultBackground
    property Component border: defaultBorder
    property Component knob: defaultKnob

    SliderModel { id: model }

    Component {
        id: defaultBackground
        Rectangle { color: backgroundColor }
    }

    Component {
        id: defaultBorder
        Rectangle {
            color: "transparent"
            border.color: borderColor
        }
    }

    Component {
        id: defaultKnob
        Rectangle { color: knobColor }
    }

    Loader {
        property alias model: model
        sourceComponent: background
        anchors.fill: parent
    }

    Loader {
        sourceComponent: border
        anchors.fill: parent
    }

    Loader {
        id: knobItem
        sourceComponent: knob
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        anchors.margins: 1
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
                    anchors.left: mouseArea.left
                    anchors.right: mouseArea.right
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
                    anchors.top: mouseArea.top
                    anchors.bottom: mouseArea.bottom
                }
            }
        ]
    }
}
