TEMPLATE = app

CONFIG -= app_bundle
CONFIG -= qt
CONFIG(release, debug|release):DEFINES += NDEBUG

QMAKE_LFLAGS += -static

LIBS += -lole32

SOURCES += main.cpp