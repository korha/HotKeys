TEMPLATE = app

CONFIG(release, debug|release):DEFINES += NDEBUG

QMAKE_LFLAGS += -static
QMAKE_CXXFLAGS += -Wpedantic

LIBS += -lole32

SOURCES += main.cpp
