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
m.set( \steps, 0 ) // round to 1/5
m.set( \count, 100 ) // change amount of sliders
m.set( \spacing, 0 )
m.set( \orientation, 1 ) // 1 = horizontal, 2 = vertical
m.set( \sliderWidth, 10 ) // fixed slider thickness
m.set( \inverted, true )
m.set( \centered, true )
m.set( \fill, false )
//...

// Now, for some oscilloscoping...

// This is just the boring SC setup part:
(
// pick your own bus here, if you want
~bus = Bus(rate: \audio, index:2, numChannels:2);
~scopeSynth = BusScopeSynth();
~scopeSynth.play(4096, ~bus, 1024);
)

// OK, let's attach to the oscilloscope GUI element:
~scope = QuickGuiElement(~gui, '/scope');
// and start it up:
(
~scope.invoke( \stop ); // in case it was already running
~scope.set( \server, ~bus.server.addr.port );
~scope.set( \buffer, ~scopeSynth.bufferIndex );
~scope.invoke( \start )
)

~scope.set(\mode, 2) // XY mode
~scope.set(\mode, 1) // Channel overlay mode
~scope.set(\mode, 0) // Normal mode
~scope.set(\yZoom, 10)
~scope.set(\xZoom, 10) // Only has effect in XY mode

~scopeSynth.setCycle(200) // change the scoping period (in sound frames)
~scope.set(\updateInterval, 50) // change the GUI update interval (in ms)
