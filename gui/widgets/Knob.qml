import QtQuick 2.0
import QuickCollider 0.1

Item
{
    id: knob
    property alias value: model.value
    property int mouseMode: 0
    property real mouseSensitivity: 0.008

    property Component pointer: defaultPointer

    RangeModel { id: model }

    Component {
        id: defaultPointer
        Rectangle {
            radius: Math.min(width, height) / 2
            color: "gray"
            border.color: "black"
            antialiasing: true
            Rectangle {
                antialiasing: true
                color: "black"
                anchors.top: parent.top;
                anchors.bottom: parent.verticalCenter;
                anchors.horizontalCenter: parent.horizontalCenter;
                width: 4;
            }
        }
    }

    Loader {
        sourceComponent: pointer;
        anchors.centerIn: parent;
        width: Math.min(parent.width, parent.height)
        height: Math.min(parent.width, parent.height)
        rotation: (knob.value - 0.5) * 300;
    }

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        property real dragValue;
        property int dragPos;
        states: [
            State {
                when: mouseMode == 0
                PropertyChanges {
                    target: mouseArea
                    onPressed: { dragValue = value; dragPos = mouse.y; }
                    onPositionChanged: {
                        value = (dragPos - mouse.y) * mouseSensitivity + dragValue;
                    }
                }
            },
            State {
                when: mouseMode == 1
                PropertyChanges {
                    target: mouseArea
                    onPressed: { dragValue = value; dragPos = mouse.x; }
                    onPositionChanged: {
                        value = (mouse.x - dragPos) * mouseSensitivity + dragValue;
                    }
                }
            },
            State {
                when: mouseMode == 2
                PropertyChanges {
                    target: mouseArea
                    onPressed: {
                        value = radialValue(mouse);
                    }
                    onPositionChanged: {
                        var new_value = radialValue(mouse);
                        if( !(new_value < 0.0 && value > 0.5) && !(new_value > 1.0 && value < 0.5) )
                            value = new_value
                    }
                }
            }
        ]
        function radialValue (mouse) {
            var dx = mouse.x - width / 2;
            var dy = mouse.y - height / 2;
            var angle = Math.atan2(dy, dx) * 180 / Math.PI;
            angle =  angle - 90;
            if (angle < 0) angle = angle + 360;
            return (angle  - 30) / 300;
        }
    }
}
