import QtQuick 2.0
import OSC 0.1
import QuickCollider 0.1
import "../gui/widgets"

Item {
    property int margin: 10;
    width: childrenRect.width + childrenRect.x + margin
    height: childrenRect.height + childrenRect.y + margin

    Row {
        spacing: 5
        x: margin
        y: margin
        Column
        {
            spacing: 5
            Row {
                spacing: 5
                Slider2D {
                    OSC.path: "/slider2D"
                    width:200;
                    height:200;
                    knobWidth: 10
                    knobHeight: 10
                    //backgroundColor: Qt.rgba(0.7,0,0)
                    //knobColor: "white"
                }

                Slider {
                    OSC.path: "/slider"
                    width: 30;
                    height: 200
                    knobSize: 10
                    // default orientation automatically chosen according to width/height
                    //orientation: Qt.Vertical
                }

                Knob {
                    OSC.path: "/knob"
                    width: 100;
                    height: 200;
                    //mouseMode:1;
                    //mouseSensitivity: 0.002
                }
            }
            MultiSlider {
                id: multiSlider
                OSC.path: "/multiSlider"
                width: 400;
                height: 200;
                count: 30;
                //spacing: 5
                //orientation: Qt.Horizontal
                //sliderWidth: 5; // ( fixed width > 0 ; auto-stretch = 0 )
                //sliderHeight: 10
                //fill: false
                //centered: true
                //backgroundColor: Qt.rgba(0.2,0.2,0.2)
                //sliderColor: Qt.rgba(0.8,0,0)
                //fillColor: Qt.rgba(0.2, 0, 0.6) // by default depends on sliderColor
            }
        }
        Column {
            spacing: 5
            Graph {
                id: graph
                OSC.path: "/graph"
                width: 400;
                height: 200
                horizontalOrder: true
            }
            Item {
                width: 400;
                height: 200
                Oscilloscope {
                    id: scope
                    OSC.path: "/scope"
                    xZoom: Math.pow(4, (xZoomSlider.value - 0.3) * 2 )
                    yZoom: Math.pow(4, (yZoomSlider.value - 0.3) * 2 )
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: grid.top
                    anchors.bottomMargin: 2
                    anchors.right: yZoomSlider.left
                    anchors.rightMargin: 2
                    //plotColors: ["red", "orange"]
                    //backgroundColor: Qt.rgba(0.3,0,0)
                    //borderColor: "white"
                }
                Slider {
                    id: yZoomSlider
                    OSC.path: "/scope/yZoomSlider"
                    width: 15
                    height: scope.height
                    anchors.right: parent.right
                    value: 0.3
                }
                Grid {
                    id: grid
                    columns: 2
                    spacing: 2
                    width: 400
                    anchors.bottom: parent.bottom
                    Text { id: xZoomLabel; text: "x zoom" }
                    Slider {
                        id: xZoomSlider
                        OSC.path: "/scope/xZoomSlider"
                        width: parent.width - x
                        height: 15
                        value: 0.3
                    }
                    Text { id: cycleLabel; text: "cycle" }
                    Slider {
                        id: cycleSlider
                        OSC.path: "/scope/cycleSlider"
                        width: parent.width - x
                        height: 15
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        for(var i=0; i < multiSlider.count; ++i)
            multiSlider.set(i, Math.random())
        for(var i=0; i<10; ++i)
            graph.add( i / 9, Math.random() );
    }
}
