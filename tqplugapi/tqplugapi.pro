#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T00:39:11
#
#-------------------------------------------------

QT       -= gui
QT += xml


TARGET = tqplugapi
TEMPLATE = lib

DEFINES += TQ_PLUGIN_API

INCLUDEPATH += \
        ../common

SOURCES += tqplugapi.cpp \
    ../common/tqbase.cpp \
    ../common/tqcond.cpp \
    ../common/tqmodels.cpp

HEADERS += tqplugapi.h\
    ../common/tqplugin_global.h \
    ../common/tqplug.h \
    ../common/tqcond.h \
    ../common/tqbase.h \
    ../common/tqmodels.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

DESTDIR = ../lib
