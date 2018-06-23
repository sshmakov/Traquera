#-------------------------------------------------
#
# Project created by QtCreator 2016-05-30T23:08:10
#
#-------------------------------------------------

CONFIG += dll
QT       -= gui
#QT += axserver
#QT += activeqt
#QT += axcontainer
CONFIG += qaxcontainer
CONFIG += qaxserver qaxserver_no_postlink
QT += script scripttools

TARGET = tqaxfactory
TEMPLATE = lib

DEF_FILE = qaxserver.def
RC_FILE  = qaxserver.rc

DEFINES += TQAXFACTORY_LIBRARY

SOURCES += tqaxfactory.cpp \
    activefactory.cpp \
    axscriptable.cpp

HEADERS += tqaxfactory.h\
        tqaxfactory_global.h \
    activefactory.h \
    axscriptable.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH += ../common
LIBS += -L../client -ltqplugapi

DESTDIR = ../client

DISTFILES += \
    qaxserver.rc \
    qaxserver.ico \
    qaxserver.def
