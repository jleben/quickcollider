import QtQuick 2.0
import OSC 0.1
import "../gui/widgets"

Item {
    property int margin: 10;
    width: childrenRect.width + childrenRect.x + margin
    height: childrenRect.height + childrenRect.y + margin

    Column {
        spacing: 5
        x: margin
        y: margin

        Row
        {
            spacing: 5

            Slider2D {
                OSC.path: "/slider2D"
                width:100;
                height:100;
                knobWidth: 10
                knobHeight: 10
                //backgroundColor: Qt.rgba(0.7,0,0)
                //knobColor: "white"
            }

            Slider {
                OSC.path: "/slider"
                width: 30;
                height: 100
                knobSize: 10
                // default orientation automatically chosen according to width/height
                //orientation: Qt.Vertical
            }

            Knob {
                OSC.path: "/knob"
                width: 50;
                height: 100;
                //mouseMode:1;
                //mouseSensitivity: 0.002
            }
        }

        MultiSlider {
            id: multiSlider
            OSC.path: "/multiSlider"
            width: 500;
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

        Graph {
            id: graph
            OSC.path: "/graph"
            width: 500;
            height: 200
            horizontalOrder: true
        }
    }

    Component.onCompleted: {
        for(var i=0; i < multiSlider.count; ++i)
            multiSlider.set(i, Math.random())
        for(var i=0; i<10; ++i)
            graph.add( i / 9, Math.random() );
    }
}
