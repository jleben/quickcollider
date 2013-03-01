QuickCollider implements an OSC interface between different processes running QML and SuperCollider code. This allows to integrate all the GUI power of Qt Quick with SuperCollider in a way that minimally interferes with the real-time requirements of the musical code.

At current stage, QuickCollider provides the following:

- 'quickcollider' - an executable that displays a QML scene and runs an OSC server for communication with the scene.

- A (growing) set of sound-control-related widgets as reusable QML components

- SC classes that wrap OSC communication into an easy API.

- Couple of demo QML scenes and corresponding SC code

### How to use the QuickCollider program

Once it is built, start up the *quickcollider* executable. It takes 2 arguments: an OSC port, and a QML scene file.

QML elements in the scene may set an "OSC.path" attached property to define the OSC path at which they are accessible. Thus, all their properties, methods and signals become accessible via OSC, by appending their name to the element path, e.g. "/anElement/aProperty".

Try *quickcollider* with demo ".qml" files in the "demo" subfolder.

### How to communicate with QuickCollider from SuperCollider

In order to use SuperCollider classes provided by QuickCollider, make the "sc" subfolder available to sclang for compilation.

Check out the demo SuperCollider code in the "demo" subfolder, which contains explanation of usage...

### How to build QuickCollider:

You will require:

- CMake
- Qt 5 (tested with RC 1)
- liblo (C library for OSC)
- SuperCollider source code (latest master branch)

*You need to tell CMake where the SuperCollider source directory is located, by setting the SC_SOURCE_DIR variable.*

