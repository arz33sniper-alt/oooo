QT       += core gui widgets
CONFIG   += c++17

SOURCES += \
    main_qt_gui.cpp \
    VoleGuiWindow.cpp \
    ../Memory.cpp \
    ../RegisterFile.cpp \
    ../Screen.cpp \
    ../Instruction.cpp \
    ../ALU.cpp \
    ../CPU.cpp \
    ../ProgramLoader.cpp \
    ../Machine.cpp

HEADERS += \
    VoleGuiWindow.h \
    ../Memory.h \
    ../RegisterFile.h \
    ../Screen.h \
    ../Instruction.h \
    ../ALU.h \
    ../CPU.h \
    ../ProgramLoader.h \
    ../Machine.h
