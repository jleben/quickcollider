QuickCollider implements an OSC interface between different processes running QML and SuperCollider
code. This allows to integrate all the GUI power of Qt Quick with SuperCollider in a way that
minimally interferes with the real-time requirements of the musical code.

At current stage, QuickCollider provides the following:

- 'quickcollider' - an executable that takes 2 arguments: an OSC port, and a QML scene file. It
displays the QML scene in a window, where each "OSC" element defines OSC endpoints for properties,
functions and signals of another associated element (its parent, by default)

- SC classes that wrap OSC communication into an easy API. In order to use them, make the "sc"
subfolder available to sclang for compilation.

- Demo QML scene and corresponding SC code in the "test" subfolder

QuickCollider depends on:

- Qt 5 (tested with RC 1)
- liblo (C library for OSC)

QuickCollider uses CMake building system.

