cmake_minimum_required(VERSION 3.17)
project(DesktopMonitorBrightness)

execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=Debug --build=missing)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src)

add_executable(DesktopMonitorBrightness
        src/DesktopMonitorBrightness.cpp
        src/DesktopMonitorBrightness.h
        src/GeocodeGrabber.hpp
        src/stdafx.cpp
        src/stdafx.h
        src/targetver.h
        src/wxui.cpp
        src/wxui.h src/dummy.cpp)
target_link_libraries(DesktopMonitorBrightness ${CONAN_LIBS})