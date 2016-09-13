#-------------------------------------------------
#
# Project created by QtCreator 2016-09-11T10:23:28
#
#-------------------------------------------------

QT       += xml

TARGET = tqgui
TEMPLATE = lib

DEFINES += TQGUI_LIBRARY

SOURCES += tqgui.cpp \
    tqsearchbox.cpp

HEADERS += tqgui.h\
        tqgui_global.h \
    tqsearchbox.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = ../client
