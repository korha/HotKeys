contains(QMAKE_HOST.arch, x86_64) {
TARGET = SwapChars64
} else {
TARGET = SwapChars32
}

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -static

SOURCES += main.cpp

HEADERS += custom.h

DEF_FILE = SwapChars.def
