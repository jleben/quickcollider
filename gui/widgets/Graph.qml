import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: root
    property real nodeSize: 16

    GraphModel {
        id: graphModel
        area: Qt.rect( nodeMargin, nodeMargin, mouseArea.width - nodeSize, mouseArea.height - nodeSize )
        nodeMargin: nodeSize / 2
        //onDataChanged: canvas.requestPaint();
    }

    Component.onCompleted: {
        for(var i=0; i<10; ++i) {
            graphModel.add( Qt.point( i/40 * root.width, (Math.random() * 0.5 + 0.5) * root.height ) )
        }
    }

    MouseArea {
        id: mouseArea
        property int pressedIndex: -1
        anchors.fill: parent
        anchors.margins: 1
        onPressed: {
            root.focus = true;
            graphModel.mousePress( Qt.point(mouse.x, mouse.y), mouse.buttons, mouse.modifiers )
        }
        onPositionChanged: {
            graphModel.mouseMove( Qt.point(mouse.x, mouse.y), mouse.buttons, mouse.modifiers )
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
    Item {
        anchors.fill: mouseArea
        Repeater {
            model: graphModel
            Rectangle {
                property int index: model.index
                antialiasing: true
                width: nodeSize
                height: nodeSize
                color: "black"
                border.color: "blue"
                border.width: selected ? 1 : 0
                radius: nodeSize / 2
                x: position.x - radius
                y: position.y - radius
            }
        }
    }
}
