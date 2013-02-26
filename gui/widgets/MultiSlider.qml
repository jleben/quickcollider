import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: root

    property alias count: mSliderModel.count;
    property alias steps: mSliderModel.steps;

    property int orientation: Qt.Vertical;
    property real spacing: 1;
    property int sliderSize: 0;

    property Component slider: defaultSlider;
    property color sliderColor: "black";

    property Component background: defaultBackground;

    signal valueChanged(int index, real value);

    Component.onCompleted: {
        mSliderModel.valueChanged.connect( valueChanged );
    }

    function setValue(index, value)
    {
        mSliderModel.setData(index, value);
    }

    MultiSliderModel {
        id: mSliderModel;
        count: 10;
        orientation: root.orientation
        bounds: Qt.rect(0,0,width,height)
    }

    Component {
        id: defaultBackground
        Rectangle {
            color: "gray"
            border.color: "black"
        }
    }

    Component {
        id: defaultSlider
        Item {
            Rectangle {
                id: indicator
                color: sliderColor;
                states: [
                    State {
                        when: orientation == Qt.Vertical
                        AnchorChanges {
                            target: indicator
                        }
                        PropertyChanges {
                            target: indicator
                            width: parent.width;
                            y: position;
                            height: parent.height - position;
                        }
                    },
                    State {
                        when: orientation == Qt.Horizontal
                        AnchorChanges {
                            target: indicator
                        }
                        PropertyChanges {
                            target: indicator
                            width: position;
                            height: parent.height;
                        }
                    }
                ]
            }
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
                    width: sliderSize
                    Binding on width {
                        when: sliderSize == 0
                        value: (count > 1) ?
                                   (root.width - ((count - 1) * spacing)) / count :
                                   root.width
                    }
                    height: root.height
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
                    height: sliderSize
                    Binding on height {
                        when: sliderSize == 0
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
        sourceComponent: (orientation == Qt.Vertical) ? horizontalArray : verticalArray
    }

    MouseArea {
        property int xOrigin;
        property int yOrigin;
        anchors.fill: sliderArray
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
