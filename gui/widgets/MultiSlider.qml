import QtQuick 2.0
import QuickCollider 0.1

Item {
    id: root

    property alias count: multiSliderModel.count;
    property alias step: multiSliderModel.step;

    property int orientation: Qt.Vertical;
    property real spacing: 1;
    property int sliderSize: 0;

    property Component slider: defaultSlider;
    property color sliderColor: "black";

    property Component background: defaultBackground;

    signal valueChanged(int index, real value);

    Component.onCompleted: {
        multiSliderModel.valueChanged.connect( valueChanged );
    }

    function setValue(index, value)
    {
        multiSliderModel.setData(index, value);
    }

    MultiSliderModel { id: multiSliderModel; count: 10 }

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
                            anchors.bottom: parent.bottom
                        }
                        PropertyChanges {
                            target: indicator
                            width: parent.width;
                            height: value * parent.height;
                        }
                    },
                    State {
                        when: orientation == Qt.Horizontal
                        AnchorChanges {
                            target: indicator
                        }
                        PropertyChanges {
                            target: indicator
                            width: value * parent.width;
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
                model: multiSliderModel
                Loader {
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
                model: multiSliderModel
                Loader {
                    property real value: model.value;
                    sourceComponent: defaultSlider
                    height: (count > 1) ?
                               (root.height - ((count - 1) * spacing)) / count :
                               root.height
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
            if (orientation == Qt.Vertical)
                setSlidersV(mouse)
            else
                setSlidersH(mouse)
        }
        onPositionChanged: {
            if (orientation == Qt.Vertical)
                setSlidersV(mouse)
            else
                setSlidersH(mouse)
            xOrigin = mouse.x;
            yOrigin = mouse.y;
        }
        function setSlidersV (mouse) {
            var origin_idx = Math.floor(xOrigin / width * multiSliderModel.count);
            var origin_val = 1 - (yOrigin / height);
            var idx = Math.floor(mouse.x / width * multiSliderModel.count);
            var val = 1 - (mouse.y / height);
            multiSliderModel.setData(origin_idx, origin_val, idx, val );
        }
        function setSlidersH (mouse) {
            var origin_idx = Math.floor(yOrigin / height * multiSliderModel.count);
            var origin_val = xOrigin / width;
            var idx = Math.floor(mouse.y / height * multiSliderModel.count);
            var val = mouse.x / width;
            multiSliderModel.setData(origin_idx, origin_val, idx, val );
        }
    }
}
