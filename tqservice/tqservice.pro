#-------------------------------------------------
#
# Project created by QtCreator 2013-12-19T23:46:46
#
#-------------------------------------------------

QT       += sql xml xmlpatterns

QT       -= gui

TARGET = tqservice
TEMPLATE = lib
CONFIG += dll
DEF_FILE = tqservice.def

DEFINES += TQSERVICE_LIBRARY

SOURCES += tqservice.cpp \
    ../common/ttutils.cpp \
    ../tracker/trkview.cpp

HEADERS += tqservice.h\
        tqservice_global.h \
    ../common/ttutils.h \
    ../tracker/trkview.h \
    ../tracker/trktool2.h \
    ../tracker/trdefs.h \
    ../tracker/tracker.h

DEFINES += CONSOLE_APP
INCLUDEPATH += $$PWD/../trktool/lib ../common ../tracker
DEPENDPATH += $$PWD/../trktool/lib
win32: LIBS += -L$$PWD/../trktool/lib -ltrktooln -lshell32 -lshlwapi

OTHER_FILES += \
    verifyn.dll \
    trkqryn.dll \
    trkapin.dll \
    trkadmn.dll \
    trindex.ini


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

#QMAKE_LFLAGS += /def:tqservice.def
