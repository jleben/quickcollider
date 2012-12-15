import QtQuick 2.0
import OSC 0.1

Item {
    width:400
    height:400
    property Item lastRectangle;

    Rectangle {
        id: addButton
        color: Qt.rgba(0,1,0,0.6)
        width: parent.width; height: 50
        Text { text: "add"; anchors.centerIn: parent }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                rectModel.append( {text: rectModel.count} )
            }
        }
    }

    Item {
        width: parent.width
        anchors.top: addButton.bottom
        anchors.bottom:  resetButton.top
        z: -1
        property Item currentItem

        Text {
            anchors.centerIn: parent
            font.pointSize: 15

            color: Qt.rgba(0.8,0.8,0.8,1);
            text: "Add an item, then click here..."
        }

        Repeater {
            id: rects
            model: ListModel { id: rectModel }

            delegate: Rectangle {
                id: item
                width:50; height:50
                color: Qt.rgba( Math.random(), Math.random(), Math.random(), 0.5);

                Text {
                    anchors.centerIn: parent
                    text: model.text
                }

                Behavior on x { SpringAnimation { spring: 10; damping: 0.1 } }
                Behavior on y { SpringAnimation { spring: 10; damping: 0.1 } }

                property var osc: OSC {
                    id: osc
                    path: "/rect" + index
                    function setX(x) { item.x = x }
                    function setY(y) { item.y = y }
                    function setPos(x, y) { item.x = x; item.y = y }
                    signal xChanged(real x)
                    signal yChanged(real y);
                    signal posChanged(real x, real y);
                }
            }

            onItemAdded: parent.currentItem = item
        }
        MouseArea {
            anchors.fill: parent
            onPressed: moveItem(parent.currentItem, mouse);
            onPositionChanged: moveItem(parent.currentItem, mouse);
            function moveItem (item, mouse) {
                item.x = mouse.x;
                item.y = mouse.y
                item.osc.xChanged(mouse.x);
                item.osc.yChanged(mouse.y);
                item.osc.posChanged(mouse.x, mouse.y);
            }
        }
    }

    Rectangle {
        id: resetButton
        color: Qt.rgba(1,0,0,0.6)
        width: parent.width; height: 50
        anchors.bottom: parent.bottom
        Text { text: "clear"; anchors.centerIn: parent }
        MouseArea {
            property bool flag
            flag: false
            anchors.fill: parent
            onPressed: rectModel.clear();
        }
    }

}
