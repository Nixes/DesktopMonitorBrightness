# DesktopMonitorBrightness
A utility to allow laptop like monitor brightness control on a desktop.

![Alt text](dmb_screen.JPG)

Features:
  - Lightwight: zero average cpu usage, less than 400k memory usage
  - Manual Brightness control from slider opened by clicking on system tray.
  - Automatic brightness based on configured sunrise and sunset times.


## Building
Requires VS2015 or above to build due to requirements for certain c++11 features.

Requires conan installed, also need to add the bincrafters remote
```
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

for local development
```
conan install . -s build_type=Debug --build=missing
```
then set your cmake environment to a debug one and build

Also requires wxWidgets libraries.

## Design
Uses wxWidgets for the interface, and json for config loading / saving.
Currently only has a windows backend for setting monitor brightness, but a linux one is planned.


## WIP
QT Now works but trying to get this to work
https://doc.qt.io/qt-5/qtwidgets-desktop-systray-example.html