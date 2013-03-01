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

            color: Qt.rgba(0,0,0,0.4);
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

                function setPos(x, y) { item.x = x; item.y = y; posChanged(x,y); }

                signal posChanged(real x, real y);

                OSC.path: "/rect" + index
            }

            onItemAdded: {
                parent.currentItem = item;
                item.x = Math.random() * (parent.width - item.width);
                item.y = Math.random() * (parent.height - item.height);
            }
        }
        MouseArea {
            anchors.fill: parent
            onPressed: moveItem(parent.currentItem, mouse);
            onPositionChanged: moveItem(parent.currentItem, mouse);
            function moveItem (item, mouse) {
                item.setPos(mouse.x, mouse.y);
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
