cmake_minimum_required(VERSION 3.16)
project(VoleMachineGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

add_executable(VoleMachineGUI
    main_qt_gui.cpp
    VoleGuiWindow.cpp
    VoleGuiWindow.h
    ../Memory.cpp
    ../RegisterFile.cpp
    ../Screen.cpp
    ../Instruction.cpp
    ../ALU.cpp
    ../CPU.cpp
    ../ProgramLoader.cpp
    ../Machine.cpp
)

if (Qt6_FOUND)
    target_link_libraries(VoleMachineGUI PRIVATE Qt6::Widgets)
else()
    target_link_libraries(VoleMachineGUI PRIVATE Qt5::Widgets)
endif()
