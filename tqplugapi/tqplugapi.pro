#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T00:39:11
#
#-------------------------------------------------

#QT       -= gui
QT += xml network sql script scripttools axserver
CONFIG += qaxcontainer
CONFIG += qaxserver dll

DEF_FILE = qaxserver.def
RC_FILE  = qaxserver.rc



TARGET = tqplugapi
TEMPLATE = lib

DEFINES += TQ_PLUGIN_API CLIENT_APP

INCLUDEPATH += \
        ../common

SOURCES += tqplugapi.cpp \
    ../common/tqbase.cpp \
    ../common/tqmodels.cpp \
    tqoauth.cpp \
    ../common/ttglobal.cpp \
    tqjson.cpp \
    activefactory.cpp

HEADERS += tqplugapi.h\
    ../common/tqplugin_global.h \
    ../common/tqplug.h \
    ../common/tqbase.h \
    ../common/tqmodels.h \
    ../common/tqplugui.h \
    ../common/tqoauth.h \
    ../common/ttglobal.h \
    ../common/tqjson.h \
    activefactory.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

DESTDIR = ../lib

win32:INCLUDEPATH += C:/OpenSSL-Win32/include
win32:LIBS += -LC:/OpenSSL-Win32/lib -LC:/OpenSSL-Win32/bin -llibeay32 -lssleay32
