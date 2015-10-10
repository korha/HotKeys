contains(QMAKE_HOST.arch, x86_64) {
TARGET = VolumeControl64
} else {
TARGET = VolumeControl32
}

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static

LIBS += -lgdi32 -lole32

SOURCES += main.cpp

DEF_FILE = VolumeControl.def
