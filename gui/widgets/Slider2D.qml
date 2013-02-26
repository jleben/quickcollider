import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property alias xValue: xModel.value
    property alias yValue: yModel.value
    property alias xInverted: xModel.inverted
    property alias yInverted: yModel.inverted
    property alias xSteps: xModel.steps
    property alias ySteps: yModel.steps
    property real knobWidth: 15
    property real knobHeight: 15
    property color knobColor: Qt.rgba(0.1,0.1,0.1)
    property color backgroundColor: "grey"
    property color borderColor: "black"

    property Component background: defaultBackground
    property Component border: defaultBorder
    property Component knob: defaultKnob

    SliderModel {
        id: xModel;
        minimumPosition: mouseArea.x + (knobWidth / 2)
        maximumPosition: mouseArea.x + mouseArea.width - (knobWidth / 2)
    }
    SliderModel {
        id: yModel
        minimumPosition: mouseArea.y + mouseArea.height - (knobHeight / 2)
        maximumPosition: mouseArea.y + (knobHeight / 2)
    }

    Component {
        id: defaultBackground
        Rectangle {
            color: backgroundColor
            Canvas {
                id: canvas
                anchors.fill: parent
                property color strokeColor:
                    Qt.rgba(borderColor.r, borderColor.g, borderColor.b, 0.3)
                onPaint: {
                    var ctx = canvas.getContext('2d');
                    ctx.strokeStyle = canvas.strokeColor;
                    ctx.moveTo( width / 2, 0 );
                    ctx.lineTo( width / 2, height );
                    ctx.moveTo( 0, height / 2 );
                    ctx.lineTo( width, height / 2 );
                    ctx.stroke();
                }
            }
        }
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
        anchors.margins: 1
        onPressed: updateValue(mouse)
        onPositionChanged: updateValue(mouse)
        function updateValue(mouse) {
            xModel.position = mouse.x;
            yModel.position = mouse.y;
        }
    }
}
