import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property bool inverted: false
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

    Mapping1D { id: mouseMapping; sourceOffset: knobSize/2 }

    Mapping1D { id: valueMapping; }

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
        NumberMapper {
            id: knobMapper
            mapping: valueMapping
            source: model.value
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        anchors.margins: 1
        onPressed: setValue(mouse)
        onPositionChanged: setValue(mouse)
        function setValue(mouse) {
            model.value = mouseMapping.map( (orientation == Qt.Vertical) ? mouse.y : mouse.x );
        }
    }

    StateGroup {
        states: [
            State {
                when: orientation == Qt.Vertical
                PropertyChanges {
                    target: mouseMapping
                    sourceRange: mouseArea.height - knobSize
                    invert: !slider.inverted
                }
                PropertyChanges {
                    target: valueMapping
                    targetOffset: mouseArea.y
                    targetRange: mouseArea.height - knobSize
                    invert: !slider.inverted
                }
                PropertyChanges {
                    target: knobItem
                    height: knobSize
                    y: knobMapper.value
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
                    target: mouseMapping
                    sourceRange: mouseArea.width - knobSize
                    invert: slider.inverted
                }
                PropertyChanges {
                    target: valueMapping
                    targetOffset: mouseArea.x
                    targetRange: mouseArea.width - knobSize
                    invert: slider.inverted
                }
                PropertyChanges {
                    target: knobItem
                    width: knobSize
                    x: knobMapper.value
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
