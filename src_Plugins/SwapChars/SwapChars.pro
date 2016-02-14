TEMPLATE = lib

QMAKE_LFLAGS += -static
QMAKE_CXXFLAGS += -Wpedantic

SOURCES += main.cpp

HEADERS += custom.h

DEF_FILE = SwapChars.def
