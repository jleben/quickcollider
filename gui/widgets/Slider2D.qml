import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: slider
    property int orientation: (width <= height) ? Qt.Vertical : Qt.Horizontal;
    property alias xValue: xModel.value
    property alias yValue: yModel.value
    property bool xInverted: false
    property bool yInverted: false
    property alias xSteps: xModel.steps
    property alias ySteps: yModel.steps
    property real knobWidth: 15
    property real knobHeight: 15
    property color knobColor: Qt.rgba(0.1,0.1,0.1)
    property color backgroundColor: Qt.rgba(0.7, 0.7, 0.7)
    property color borderColor: "black"

    property Component background: defaultBackground
    property Component border: defaultBorder
    property Component knob: defaultKnob

    SliderModel { id: xModel; }
    SliderModel { id: yModel  }

    Mapping2D {
        id: mouseMapping
        sourceRect: Qt.rect( knobWidth / 2,
                             knobHeight / 2,
                             mouseArea.width - knobWidth,
                             mouseArea.height - knobHeight )
        invertX: xInverted
        invertY: !yInverted
    }

    Mapping2D {
        id: valueMapping
        targetRect: Qt.rect(mouseArea.x,
                            mouseArea.y,
                            mouseArea.width - knobWidth,
                            mouseArea.height - knobHeight )
        invertX: xInverted
        invertY: !yInverted
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
        x: valueMapper.x
        y: valueMapper.y
        PointMapper {
            id: valueMapper
            mapping: valueMapping
            source: Qt.point(xModel.value, yModel.value)
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        anchors.margins: 1
        onPressed: updateValue(mouse)
        onPositionChanged: updateValue(mouse)
        function updateValue(mouse) {
            var value = mouseMapping.map( Qt.point(mouse.x, mouse.y) );
            xModel.value = value.x;
            yModel.value = value.y;
        }
    }
}
