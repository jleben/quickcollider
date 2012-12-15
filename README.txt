QuickCollider implements an OSC interface between different processes
running QML and SuperCollider code. This allows to integrate all the
GUI power of Qt Quick with SuperCollider in a way that minimally
interferes with the real-time requirements of the musical code.

At current stage, QuickCollider provides the following:

- 'quickcollider' - an executable that takes 2 arguments: an OSC port,
  and a QML scene file. It displays the scene in a window, creating
  OSC listen/notify endpoints for properties, signals and slots of each
  "OSC" element in the scene.

- SC classes that wrap OSC communication into an easy API.
  In order to use them, make the "sc" subfolder available to sclang
  for compilation.

- Demo QML scene and corresponding SC code in the "test" subfolder

QuickCollider depends on:

- Qt 5 (tested with RC 1)
- liblo (C library for OSC)

QuickCollider uses CMake building system.

