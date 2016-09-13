#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T00:39:11
#
#-------------------------------------------------

CONFIG += dll

#QT       -= gui
QT += xml network sql script scripttools xmlpatterns
#QT += axserver
#QT += axcontainer
CONFIG += qaxcontainer
#CONFIG += qaxserver

#DEF_FILE = qaxserver.def
#RC_FILE  = qaxserver.rc



TARGET = tqplugapi
TEMPLATE = lib

DEFINES += TQ_PLUGIN_API CLIENT_APP

INCLUDEPATH += \
        ../common \
        ../tqaxfactory

SOURCES += tqplugapi.cpp \
    ../common/tqbase.cpp \
    ../common/tqmodels.cpp \
    tqoauth.cpp \
    ../common/ttglobal.cpp \
    tqjson.cpp \
    ../common/tqviewcontroller.cpp \
    tqnetaccessmanager.cpp \
    ../common/tqdebug.cpp \
    tqplug.cpp \
    ../common/tqfielddelegate.cpp

HEADERS += tqplugapi.h\
    ../common/tqplugin_global.h \
    ../common/tqplug.h \
    ../common/tqbase.h \
    ../common/tqmodels.h \
    ../common/tqplugui.h \
    ../common/tqoauth.h \
    ../common/ttglobal.h \
    ../common/tqjson.h \
    ../common/tqviewcontroller.h \
    tqnetaccessmanager.h \
    ../common/tqdebug.h \
    ../common/ttutils.h \
    ../common/tqfielddelegate.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

DESTDIR = ../client

win32:INCLUDEPATH += C:/OpenSSL-Win32/include
win32:LIBS += -LC:/OpenSSL-Win32/lib -LC:/OpenSSL-Win32/bin -llibeay32 -lssleay32
