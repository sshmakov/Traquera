#-------------------------------------------------
#
# Project created by QtCreator 2013-08-17T13:53:27
#
#-------------------------------------------------

QT       += core sql xml xmlpatterns network

QT       -= gui

TARGET = trindex
CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    ../tracker/trkview.cpp \
    ../tracker/trkcond.cpp \
    ../common/ttutils.cpp


HEADERS += \
    main.h \
    ../tracker/trkview.h \
    ../tracker/trkcond.h \
    ../common/ttutils.h

DEFINES += CONSOLE_APP
INCLUDEPATH += $$PWD/../trktool/lib ../common ../tracker
DEPENDPATH += $$PWD/../trktool/lib
win32: LIBS += -L$$PWD/../trktool/lib -ltrktooln -lshell32 -lshlwapi -lodbc32 -L../lib -ltqplugapi   -ltqcondapi \
    -L../plugins/tracker -ltracker

OTHER_FILES += \
    verifyn.dll \
    trkqryn.dll \
    trkapin.dll \
    trkadmn.dll \
    trindex.ini

build_pass:CONFIG(debug, debug|release) {
    DEFINES += USE_VLD
    win32: INCLUDEPATH += "C:/Program Files (x86)/Visual Leak Detector/include"
    win32: LIBS += -L"C:/Program Files (x86)/Visual Leak Detector/lib/win32"
} else {
    DEFINES -= USE_VLD
}
