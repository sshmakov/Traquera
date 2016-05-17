#-------------------------------------------------
#
# Project created by QtCreator 2014-05-11T23:36:43
#
#-------------------------------------------------

QT       += xml
CONFIG += qaxcontainer

TARGET = msplans
TEMPLATE = lib

DEFINES += MSPLANS_LIBRARY

SOURCES += plans.cpp \
 planfiles.cpp       \
 planfilesform.cpp   \
 planproxy.cpp       \
 tqpluginwidget.cpp    \
 tqplanswidget.cpp   \
 project.cpp \
    projectpage.cpp

HEADERS += plans.h\
 msplans_global.h    \
 planfiles.h         \
 planfilesform.h     \
 planproxy.h         \
 tqpluginwidget.h    \
 tqplanswidget.h     \
 project.h \
    projectpage.h


FORMS = \
  planfilesform.ui \
  prjfiledlg.ui \
    projectpage.ui

DESTDIR=../client/plugins/msplans

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

datafolder.source = redistribute
datafolder.target = .
DEPLOYMENTFOLDERS = datafolder

INCLUDEPATH += ../common

include(deploy.pri)
qtcAddDeployment()

OTHER_FILES += \
    redistribute/data/project.xml \
    redistribute/data/scr2prj.xml \
    redistribute/data/scr2prj.xml \
    redistribute/data/project.xml \
    redistribute/data/plugin.xml

RESOURCES += \
    msplans.qrc

LIBS += -L../lib -ltqplugapi

