QuickCollider implements an OSC interface between different processes running QML and SuperCollider code. This allows to integrate all the GUI power of Qt Quick with SuperCollider in a way that minimally interferes with the real-time requirements of the musical code.

At current stage, QuickCollider provides the following:

- 'quickcollider' - an executable that displays a QML scene and runs an OSC server for communication with the scene.

- A (growing) set of sound-control-related widgets as reusable QML components

- SC classes that wrap OSC communication into an easy API.

- Couple of demo QML scenes and corresponding SC code in the "test" subfolder

** How to use the QuickCollider program

It takes 2 arguments: an OSC port, and a QML scene file. QML elements in the scene may set an "OSC.path" attached property to define the OSC path at which they are accessible. Thus, all their properties, methods and signals become accessible via OSC, by appending their name to the element path, e.g. "/anElement/aProperty".

** How to communicate with QuickCollider from SuperCollider

In order to use SuperCollider classes provided by QuickCollider, make the "sc" subfolder available to sclang for compilation.

Check out the demo SuperCollider document, which contains explanation of usage...

** Prerequisits

QuickCollider depends on:

- Qt 5 (tested with RC 1)
- liblo (C library for OSC)

QuickCollider uses CMake building system.

