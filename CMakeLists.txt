cmake_minimum_required(VERSION 3.17)
project(DesktopMonitorBrightness)

execute_process(COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=Debug --build=missing)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# turn on auto including of QT resource files referenced in add_executable
set(CMAKE_AUTORCC ON)


include_directories(src)

SET(WINDOWS_RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sample.rc)

# Find the QtCore library
find_package(Qt5Core CONFIG REQUIRED)

add_executable(DesktopMonitorBrightness ${WINDOWS_RESOURCE_PATH}
        src/DesktopMonitorBrightness.cpp
        src/DesktopMonitorBrightness.h
        src/GeocodeGrabber.hpp
        src/stdafx.cpp
        src/stdafx.h
        src/targetver.h
        src/qtmain.cpp
        src/brightnessControlWindow.h
        src/brightnessControlWindow.cpp
        src/taskbar-icon.qrc
        )
target_link_libraries(DesktopMonitorBrightness ${CONAN_LIBS} Qt5::Core)

configure_file(${CMAKE_CURRENT_BINARY_DIR}/qt.conf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qt.conf COPYONLY)