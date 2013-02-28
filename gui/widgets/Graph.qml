import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: root
    property real nodeSize: 8
    property alias horizontalOrder: graphModel.horizontalOrder
    function add(x, y)
    {
        graphModel.addValue(x, y);
    }

    GraphModel {
        id: graphModel
        property real margin: nodeSize / 2
        area: Qt.rect(margin,
                      margin,
                      globalMouseArea.width - nodeSize,
                      globalMouseArea.height - nodeSize)
        //onDataChanged: canvas.requestPaint();
    }

    MouseArea {
        id: globalMouseArea
        anchors.fill: parent
        anchors.margins: 1
        property bool grabbed: false
        onPressed: {
            root.focus = true;
            if (mouse.modifiers & Qt.ControlModifier) {
                var index = graphModel.addPosition(mouse.x, mouse.y);
                graphModel.select(index);
                graphModel.grabSelection(mouse.x, mouse.y);
                grabbed = true;
            }
            else {
                graphModel.deselectAll();
                grabbed = false;
            }
        }
        onPositionChanged: {
            if (grabbed)
                graphModel.moveSelection(mouse.x, mouse.y);
        }
    }

    Keys.onPressed: {
        graphModel.keyPress( event.key, event.modifiers );
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "gray"
        border.color: root.activeFocus ? "blue" : "black"
    }

    GraphPlotter2 {
        model: graphModel
        anchors.fill: parent
    }

/*
    Canvas {
        id: canvas
        anchors.fill: mouseArea
        property color strokeColor:
            //Qt.rgba(borderColor.r, borderColor.g, borderColor.b, 0.3)
            Qt.rgba(0,0,0,0.3)
        antialiasing: true
        onPaint: {
            var ctx = canvas.getContext('2d');
            ctx.save();
            ctx.clearRect(0,0,canvas.width,canvas.height);
            ctx.strokeStyle = canvas.strokeColor;
            ctx.beginPath();
            var count = graphModel.count();
            if (count < 2)
                return;
            var pos = graphModel.data(0, GraphModel.PositionRole);
            ctx.moveTo(pos.x, pos.y);
            for (var idx=1; idx < count; ++idx)
            {
                pos = graphModel.data(idx, GraphModel.PositionRole);
                ctx.lineTo(pos.x, pos.y);
            }
            ctx.stroke();
            ctx.restore();
        }
    }
*/
    Component {
        id: node
        Rectangle {
            id: circle
            antialiasing: true
            width: nodeSize
            height: nodeSize
            color: "black"
            radius: nodeSize / 2
            x: position.x - radius
            y: position.y - radius
            Rectangle {
                id: ring
                antialiasing: true
                anchors.fill: parent
                anchors.margins: -2
                radius: width / 2
                color: "transparent"
                border.color: "blue"
                border.width: 2
                opacity: 0.0
                states: [
                    State {
                        when: selected
                        PropertyChanges { target: ring; opacity: 1.0 }
                    },
                    State {
                        when: mouseArea.containsMouse
                        PropertyChanges { target: ring; opacity: 0.5 }
                    }
                ]
                transitions: Transition {
                    NumberAnimation { properties:"opacity"; duration: 180 }
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                anchors.margins: -4
                hoverEnabled: true
                property bool grabbed: false
                onPressed: {
                    var pos = mapToItem(globalMouseArea, mouse.x, mouse.y);
                    if (mouse.modifiers & Qt.ControlModifier) {
                        mouse.accepted = false;
                        return;
                    }
                    grabbed = true;
                    if (mouse.modifiers & Qt.ShiftModifier) {
                        if (selected) {
                            graphModel.deselect(model.index)
                            grabbed = false;
                        }
                        else
                            graphModel.select(model.index, false)
                    }
                    else if (!selected) {
                        graphModel.select(model.index, true)
                    }
                    if (grabbed)
                        graphModel.grabSelection(pos.x, pos.y);
                }
                onPositionChanged: {
                    if (mouse.buttons && grabbed) {
                        var pos = mapToItem(globalMouseArea, mouse.x, mouse.y);
                        graphModel.moveSelection(pos.x, pos.y);
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: globalMouseArea
        Repeater {
            model: graphModel
            delegate: node
        }
    }
}
