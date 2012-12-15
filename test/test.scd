// assuming you have started quickcollider
// with port 12345, and the test QML scene provided in "test/scene.qml"

~gui = QuickGui(NetAddr("localhost", 12345));

// Try changing the number after '/rect', as you add rectangles in the QML scene:
~elem = QuickGuiElement(~gui, '/rect0');
~elem.subscribe( \posChanged, { |...a| a.postln; } );
~elem.subscribe( \xChanged, { |...a| a.postln; } );
~elem.subscribe( \yChanged, { |...a| a.postln; } );
~elem.unsubscribe( \xChanged, \yChanged );
~elem.unsubscribeAll;
~elem.send( \setX, 300.rand )
~elem.send( \setY, 300.rand )
~elem.send( \setPos, 300.rand, 300.rand )
