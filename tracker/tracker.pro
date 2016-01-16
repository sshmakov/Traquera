#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T08:38:15
#
#-------------------------------------------------

QT       += network sql xml

TARGET = tracker
TEMPLATE = lib

DEFINES += TRACKER_LIBRARY

SOURCES += trkplugin.cpp \
    trkview.cpp \
    trkcondwidgets.cpp \
    trkcond.cpp \
    ../common/ttutils.cpp

HEADERS += trkplugin.h\
        tracker_global.h \
    trkview.h \
    trktool2.h \
    trkcondwidgets.h \
    trdefs.h \
    tracker_global.h \
    trkcond.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    trkdatesdlg.ui \
    trkchangedlg.ui

INCLUDEPATH += \
        ../trktool/lib \
        ../common

win32: LIBS += -L$$PWD/../trktool/lib/ -ltrktooln -lshell32 -lshlwapi -L../lib -ltqplugapi  -ltqcondapi

DEFINES += CLIENT_APP
DESTDIR = ../plugins/tracker

TRANSLATIONS = tracker_ru.ts tracker_en.ts

CODECFORTR = utf-8

OTHER_FILES += \
    tracker_ru.ts
