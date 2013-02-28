// Assuming you have started quickcollider with port 12345
// and using the test QML scene provided in "test/dynamic.qml"

// Create a connection with the QML scene:
~gui = QuickGui(NetAddr("localhost", 12345));

// After adding a rectangle in the QML scene, create an object
// corresponding to that element:
~elem = QuickGuiElement(~gui, '/rect0');
// After adding a new rectangle, change the number
// after '/rect' above, to work with the new element.

// Set values of properties:
~elem.set( \x, 300.rand );
~elem.set( \y, 300.rand );
// Invoke any function (method) in the context of the element:
~elem.invoke( \setPos, 300.rand, 300.rand )

// Subscribe to property changes:
~elem.subscribe( \x, { |...a| a.postln; } );
// ...or to signals:
~elem.subscribe( \posChanged, { |...a| a.postln; } );
// Remove individual subscriptions:
~elem.unsubscribe( \posChanged );
// ...or all subscriptions:
~elem.unsubscribeAll;
