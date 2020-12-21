cmake_minimum_required(VERSION 3.17)
project(DesktopMonitorBrightness)

execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=Debug --build=missing)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src)

SET(WINDOWS_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sample.rc)

add_executable(DesktopMonitorBrightness WIN32
        src/DesktopMonitorBrightness.cpp
        src/DesktopMonitorBrightness.h
        src/GeocodeGrabber.hpp
        src/stdafx.cpp
        src/stdafx.h
        src/targetver.h
        src/wxui.cpp
        src/wxui.h ${WINDOWS_RESOURCE_PATH})
target_link_libraries(DesktopMonitorBrightness ${CONAN_LIBS})