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
                    width:150;
                    height:150;
                    knobWidth: 10
                    knobHeight: 10
                    //backgroundColor: Qt.rgba(0.7,0,0)
                    //knobColor: "white"
                }

                Slider {
                    OSC.path: "/slider"
                    width: 25;
                    height: 150
                    knobSize: 10
                    // default orientation automatically chosen according to width/height
                    //orientation: Qt.Vertical
                }

                Knob {
                    OSC.path: "/knob"
                    width: 70;
                    height: 150;
                    //mouseMode:1;
                    //mouseSensitivity: 0.002
                }
            }
            MultiSlider {
                id: multiSlider
                OSC.path: "/multiSlider"
                width: 280;
                height: 150;
                count: 15;
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
            Graph {
                id: graph
                OSC.path: "/graph"
                width: 280;
                height: 150
                horizontalOrder: true
            }
        }
        Column {
            spacing: 5
            Waveform {
                id: waveform
                OSC.path: "/waveform"
                width: 300
                height: 150
            }
            Oscilloscope {
                id: scope
                OSC.path: "/scope"
                width: 300
                height: 150
                xZoom: Math.pow(4, (xZoomSlider.value - 0.3) * 2 )
                yZoom: Math.pow(4, (yZoomSlider.value - 0.3) * 2 )
                //plotColors: ["red", "orange"]
                //backgroundColor: Qt.rgba(0.3,0,0)
                //borderColor: "white"
            }
            Grid {
                id: grid
                columns: 2
                spacing: 2
                width: 300
                property real sliderWidth: {
                    width - Math.max(yZoomLabel.width,
                                     xZoomLabel.width,
                                     cycleLabel.width) - spacing;
                }
                Slider {
                    id: yZoomSlider
                    OSC.path: "/scope/yZoomSlider"
                    width: grid.sliderWidth
                    height: 15
                    value: 0.3
                }
                Text { id: yZoomLabel; text: "y zoom" }
                Slider {
                    id: xZoomSlider
                    OSC.path: "/scope/xZoomSlider"
                    width: grid.sliderWidth
                    height: 15
                    value: 0.3
                }
                Text { id: xZoomLabel; text: "x zoom" }
                Slider {
                    id: cycleSlider
                    OSC.path: "/scope/cycleSlider"
                    width: grid.sliderWidth
                    height: 15
                }
                Text { id: cycleLabel; text: "cycle" }
            }

        }
    }

    Component.onCompleted: {
        for(var i=0; i < multiSlider.count; ++i)
            multiSlider.set(i, Math.random())
        for(var i=0; i<8; ++i)
            graph.add( i / 7, Math.random() );
    }
}
