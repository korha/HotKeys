contains(QMAKE_HOST.arch, x86_64) {
TARGET = CapsSwitch64
} else {
TARGET = CapsSwitch32
}

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static

SOURCES += main.cpp

DEF_FILE = CapsSwitch.def
