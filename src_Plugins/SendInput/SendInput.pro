contains(QMAKE_HOST.arch, x86_64) {
TARGET = SendInput64
} else {
TARGET = SendInput32
}

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static

SOURCES += main.cpp

DEF_FILE = SendInput.def
