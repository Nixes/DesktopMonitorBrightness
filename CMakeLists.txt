cmake_minimum_required(VERSION 3.15)
project(DesktopMonitorBrightness)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(DesktopMonitorBrightness
        src/DesktopMonitorBrightness.cpp
        src/DesktopMonitorBrightness.h
        src/GeocodeGrabber.hpp
        src/stdafx.cpp
        src/stdafx.h
        src/targetver.h
        src/wxui.cpp
        src/wxui.h)
