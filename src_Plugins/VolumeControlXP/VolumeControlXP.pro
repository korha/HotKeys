contains(QMAKE_HOST.arch, x86_64) {
TARGET = VolumeControlXP64
} else {
TARGET = VolumeControlXP32
}

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static

LIBS += -lwinmm -lgdi32

SOURCES += main.cpp

DEF_FILE = VolumeControlXP.def
