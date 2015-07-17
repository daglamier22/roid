#-------------------------------------------------
#
# Project created by QtCreator 2015-06-25T09:03:58
#
#-------------------------------------------------

QT       -= core gui

CONFIG(debug, debug|release) {
TARGET = engined
DESTDIR = ../../../lib
}
CONFIG(release, debug|release) {
TARGET = engine
DESTDIR = ../../../lib
DEFINES += NDEBUG
}

TEMPLATE = lib
CONFIG += staticlib

SOURCES += engine.cpp \
    utilities/rng.cpp \
    gameapp.cpp \
    utilities/logger.cpp \
    utilities/tinystr.cpp \
    utilities/tinyxml.cpp \
    utilities/tinyxmlerror.cpp \
    utilities/tinyxmlparser.cpp \
	resourcecache/zipfile.cpp

HEADERS += engine.h \
    utilities/rng.h \
    gameapp.h \
    utilities/logger.h \
    utilities/tinystr.h \
    utilities/tinyxml.h \
	resourcecache/zipfile.h \
    resourcecache/rescache_interfaces.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_CXXFLAGS += -std=c++11
