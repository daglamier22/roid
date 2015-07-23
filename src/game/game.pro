TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()


CONFIG(debug, debug|release) {
unix:!macx: LIBS += -L$$PWD/../../lib/ -lengined

INCLUDEPATH += $$PWD/../engine
DEPENDPATH += $$PWD/../../

unix:!macx: PRE_TARGETDEPS += $$PWD/../../lib/libengined.a
}

CONFIG(release, debug|release) {
unix:!macx: LIBS += -L$$PWD/../../lib/ -lengine

INCLUDEPATH += $$PWD/../engine
DEPENDPATH += $$PWD/../../

unix:!macx: PRE_TARGETDEPS += $$PWD/../../lib/libengine.a

DESTDIR = ../../../game
}

LIBS += -lz -ltbb -lXm -lXt

DISTFILES += \
	../../game/logging.xml
