******
Build Instructions
******

1) Update the android project::

    $ android update project --target <target_ID> --path . --library ../sc_demo_libraries/GraphView --name Hpx_HPIIF

2) Issue ant build command::

    $ ant debug

3) Get the resulting .apk file onto your device::

    $ adb install bin/Hpx_HPIIF-debug.apk
