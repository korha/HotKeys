QT -= core gui

TARGET = VolumeControl

TEMPLATE = lib

QMAKE_LFLAGS += -static

LIBS += -lole32 -lgdi32

SOURCES += main.cpp

DEF_FILE = VolumeControl.def
