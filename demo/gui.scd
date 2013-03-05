// Assuming you have started quickcollider with port 12345
// and using the test QML scene provided in "test/gui.qml"

~gui = QuickGui(NetAddr("localhost", 12345));

x = QuickGuiElement(~gui, '/slider2D');
x.subscribe( \xValue, { |...a| a.postln })
x.subscribe( \yValue, { |...a| a.postln })
x.set( \xSteps, 8 ) // round to 1/8
x.set( \ySteps, 8 )
//...

x = QuickGuiElement(~gui, '/slider');
x.subscribe( \value, { |...a| a.postln })
x.set( \knobSize, 30 )
//...

k = QuickGuiElement(~gui, '/knob');
k.subscribe( \value, { |...a| a.postln })
k.set( \steps, 5 ) // round to 1/5
k.set( \mouseMode, 2 ) // radial interaction
//...

m = QuickGuiElement(~gui, '/multiSlider');
m.subscribe(\valueChanged, { |...a| a.postln } )
m.set( \steps, 5 ) // round to 1/5
m.set( \count, 30 ) // change amount of sliders
m.set( \spacing, 0 )
m.set( \orientation, 1 ) // 1 = horizontal, 2 = vertical
m.set( \sliderWidth, 10 ) // fixed slider thickness
m.set( \inverted, true )
m.set( \centered, true )
m.set( \fill, false )
//...

w = QuickGuiElement(~gui, '/waveform');
w.set( \file, Platform.resourceDir++"/sounds/a11wlk01.wav")

// Now, for some oscilloscoping...
// QuickOscilloscope is a subclass of QuickGuiElement that
// does the dirty work for you:
~scope = QuickOscilloscope(~gui, '/scope')
~scope.start
~scope.stop

// Connect the "cycle" slider to change the scoping period:
~cycleSlider = QuickGuiElement(~gui, '/scope/cycleSlider');
(
~cycleSlider.subscribe( \value, { |path, value|
	var cycle = value.pow(2) * (~scope.maxCycle - 50) + 50;
	~scope.cycle = cycle;
});
)

// Remote control appearance:
~scope.set(\mode, 2) // XY mode
~scope.set(\mode, 1) // Channel overlay mode
~scope.set(\mode, 0) // Normal mode
~scope.set(\updateInterval, 150) // change the GUI update interval (in ms)

