QT -= core gui

TARGET = VolumeControlXP

TEMPLATE = lib

QMAKE_LFLAGS += -static

LIBS += -lwinmm -lgdi32

SOURCES += main.cpp

DEF_FILE = VolumeControlXP.def
