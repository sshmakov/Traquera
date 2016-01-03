#-------------------------------------------------
#
# Project created by QtCreator 2015-08-08T10:23:53
#
#-------------------------------------------------

QT       += xml network webkit

TARGET = jira
TEMPLATE = lib

DEFINES += JIRA_LIBRARY


SOURCES += jiradb.cpp \
    qt-json/json.cpp \
    jiraoptions.cpp \
    webform.cpp \
    jiralogin.cpp

HEADERS += jiradb.h\
        jira_global.h \
    qt-json/json.h \
    jiraoptions.h \
    webform.h \
    jiralogin.h

INCLUDEPATH += ../common

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:LIBS += -L../lib -ltqplugapi
DESTDIR = ../plugins/jira

FORMS += \
    jiraoptions.ui \
    webform.ui \
    jiralogin.ui

OTHER_FILES += \
    mykey.pub \
    mykey.pem
