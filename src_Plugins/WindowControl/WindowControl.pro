contains(QMAKE_HOST.arch, x86_64) {
TARGET = WindowControl64
} else {
TARGET = WindowControl32
}

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static

SOURCES += main.cpp

DEF_FILE = WindowControl.def
