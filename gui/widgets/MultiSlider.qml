import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: root

    property alias count: mSliderModel.count;
    property alias steps: mSliderModel.steps;

    property int orientation: Qt.Vertical;
    property real spacing: 1;
    property int sliderWidth: 0;
    property int sliderHeight: 3
    property bool fill: true
    property bool inverted: false
    property bool centered: false
    property color sliderColor: Qt.rgba(0.1,0.1,0.1,1);
    property color fillColor: Qt.rgba(sliderColor.r, sliderColor.g, sliderColor.b, 0.4);
    property color backgroundColor: "gray"

    property Component slider: defaultSlider;
    property Component background: defaultBackground;

    function set(index, value)
    {
        mSliderModel.setData(index, value);
    }

    signal valueChanged(int index, real value);

    Component.onCompleted: {
        mSliderModel.valueChanged.connect( valueChanged );
    }

    MultiSliderModel {
        id: mSliderModel;
        count: 10;
        orientation: root.orientation
        bounds:
        {
            var r = sliderArray.childrenRect;
            if (orientation == Qt.Vertical)
                Qt.rect(r.x, r.y + sliderHeight/2,
                        r.width, r.height - sliderHeight)
            else
                Qt.rect(r.x + sliderHeight/2, 0,
                        r.width - sliderHeight, r.height)
        }
        property real reference: centered ? 0.5 : (inverted ? 1 : 0)
    }

    Component {
        id: defaultBackground
        Rectangle {
            color: backgroundColor
            border.color: "black"
        }
    }

    Component {
        id: defaultSlider
        Item {
            Rectangle {
                id: fill
                visible: root.fill
                color: fillColor;
            }
            Rectangle {
                id: handle
                color: sliderColor;
            }
            states: [
                State {
                    when: orientation == Qt.Vertical
                    PropertyChanges {
                        target: fill
                        width: parent.width;
                        y: centered ? Math.min(0.5 * parent.height, position)
                                    : (inverted ? 0 : position)
                        height: centered ? Math.abs(0.5 * parent.height - position)
                                         : (inverted ? position : parent.height - position)
                    }
                    PropertyChanges {
                        target: handle
                        width: parent.width;
                        height: sliderHeight
                        y: position - (sliderHeight / 2)
                    }
                },
                State {
                    when: orientation == Qt.Horizontal
                    PropertyChanges {
                        target: fill
                        height: parent.height;
                        x: centered ? Math.min(0.5 * parent.width, position)
                                    : (inverted ? position : 0)
                        width: centered ? Math.abs(0.5 * parent.width - position)
                                        : (inverted ? parent.width - position : position)
                    }
                    PropertyChanges {
                        target: handle
                        width: sliderHeight
                        height: parent.height
                        x: position - (sliderHeight / 2)
                    }
                }
            ]
        }
    }

    Loader {
        sourceComponent: background
        anchors.fill: parent
    }

    ArrayLayout {
        id: sliderArray
        anchors.fill: parent
        anchors.margins: 1
        clip: root.sliderWidth > 0
        orientation: root.orientation == Qt.Vertical ? Qt.Horizontal : Qt.Vertical
        extent: root.sliderWidth
        spacing: root.spacing
        Repeater {
            id: repeater
            model: mSliderModel
            delegate: defaultSlider
        }
    }

    MouseArea {
        id: mouseArea
        property int xOrigin;
        property int yOrigin;
        anchors.fill: parent
        anchors.margins: 1
        onPressed: {
            xOrigin = mouse.x;
            yOrigin = mouse.y;
            mSliderModel.setPositions(xOrigin, yOrigin, mouse.x, mouse.y)
        }
        onPositionChanged: {
            mSliderModel.setPositions(xOrigin, yOrigin, mouse.x, mouse.y)
            xOrigin = mouse.x;
            yOrigin = mouse.y;
        }
    }
}
