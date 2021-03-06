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

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

TRANSLATIONS = \
    redistribute/tracker/lang/tracker.ru.ts \
    redistribute/tracker/lang/tracker.en.ts

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

win32: LIBS += -L$$PWD/../trktool/lib/ -ltrktooln -lshell32 -lshlwapi \
    -L../lib -L../client -ltqplugapi  -ltqcondapi

DEFINES += CLIENT_APP
DESTDIR = ../client/plugins/tracker

OTHER_FILES += \
    redistribute/tracker/lang/tracker.ru.ts \
    redistribute/tracker/lang/tracker.en.ts

redistribute.source = redistribute/tracker
redistribute.target = ../client/plugins
DEPLOYMENTFOLDERS = redistribute

include(deploy.pri)
qtcAddDeployment()

