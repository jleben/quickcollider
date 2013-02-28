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
        bounds: {
            var extent;
            if (sliderWidth > 0) {
                extent = count * sliderWidth;
                if (count > 0)
                    extent = extent + (count - 1) * spacing;
            } else {
                extent = (orientation == Qt.Vertical) ? sliderArray.width : sliderArray.height;
            }
            if (orientation == Qt.Vertical)
                Qt.rect(0, sliderHeight/2,
                        extent, mouseArea.height - sliderHeight)
            else
                Qt.rect(sliderHeight/2, 0,
                        mouseArea.width - sliderHeight, extent)
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
                        width: position;
                        height: parent.height;
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

    Component {
        id: horizontalArray
        Row {
            spacing: root.spacing
            Repeater {
                id: repeater
                model: mSliderModel
                Loader {
                    property real position: model.position;
                    property real value: model.value;
                    sourceComponent: defaultSlider
                    width: sliderWidth
                    Binding on width {
                        when: sliderWidth == 0
                        value: (count > 1) ?
                                   (parent.width - ((count - 1) * spacing)) / count :
                                   parent.width
                    }
                    height: parent.height
                }
            }
        }
    }

    Component {
        id: verticalArray
        Column {
            spacing: root.spacing
            Repeater {
                id: repeater
                model: mSliderModel
                Loader {
                    property real position: model.position;
                    property real value: model.value;
                    sourceComponent: defaultSlider
                    height: sliderWidth
                    Binding on height {
                        when: sliderWidth == 0
                        value: (count > 1) ?
                                   (root.height - ((count - 1) * spacing)) / count :
                                   root.height
                    }
                    width: root.width
                }
            }
        }
    }


    Loader {
        sourceComponent: background
        anchors.fill: parent
    }

    Loader {
        id: sliderArray
        anchors.fill: parent
        anchors.margins: 1
        sourceComponent: (orientation == Qt.Vertical) ? horizontalArray : verticalArray
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
