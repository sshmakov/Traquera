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
    jiralogin.cpp \
    jiraquerydialog.cpp \
    jiraqry.cpp \
    jirafinduser.cpp \
    jirausermodel.cpp

HEADERS += jiradb.h\
        jira_global.h \
    qt-json/json.h \
    jiraoptions.h \
    webform.h \
    jiralogin.h \
    jiraquerydialog.h \
    jiraqry.h \
    jirafinduser.h \
    jirausermodel.h

INCLUDEPATH += ../common

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:LIBS += -L../lib -L../client -ltqplugapi -ltqcondapi -ltqgui
DESTDIR = ../client/plugins/jira

CODECFORTR = UTF-8

TRANSLATIONS = \
    redistribute/jira/lang/jira.ru.ts \
    redistribute/jira/lang/jira.en.ts


FORMS += \
    jiraoptions.ui \
    webform.ui \
    jiralogin.ui \
    jiraquerydialog.ui \
    jirafinduser.ui

OTHER_FILES += \
    mykey.pub \
    mykey.pem \
    jira.ini \
    data/issue.xq \
    redistribute/jira/data/issue.xq \
    redistribute/jira/lang/jira.ru.qm \
    redistribute/jira/lang/jira.en.qm \
    redistribute/jira/lang/jira.ru.ts \
    redistribute/jira/lang/jira.en.ts

datafolder.source = redistribute/jira
datafolder.target = ../client/plugins
DEPLOYMENTFOLDERS = datafolder

include(deploy.pri)
qtcAddDeployment()
