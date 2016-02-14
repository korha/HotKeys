TEMPLATE = lib

QMAKE_LFLAGS += -static
QMAKE_CXXFLAGS += -Wpedantic

LIBS += -lgdi32 -lwinmm

SOURCES += main.cpp

DEF_FILE = VolumeControlXP.def
