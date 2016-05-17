#-------------------------------------------------
#
# Project created by QtCreator 2015-10-18T21:54:10
#
#-------------------------------------------------

QT += xml
TARGET = tqcondapi
TEMPLATE = lib

DEFINES += TQCONDAPI_LIBRARY

SOURCES += tqcondapi.cpp \
    ../common/tqcondwidgets.cpp \
    ../common/tqcond.cpp \
    ../common/tqqrywid.cpp

HEADERS += tqcondapi.h\
    ../common/tqcondwidgets.h \
    ../common/tqcond.h \
    ../common/tqcondapi_global.h \
    ../common/tqqrywid.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH += \
        ../common
win32:LIBS += -L../client -ltqplugapi
DESTDIR = ../client

FORMS += \
    tqqrywid.ui

RESOURCES += \
    ../client/resources/tracks.qrc
