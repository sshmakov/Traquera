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
    projectpage.h \
    ../common/tqplug.h


FORMS = \
  planfilesform.ui \
  prjfiledlg.ui \
    projectpage.ui

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

datafolder.source = data
datafolder.target = .
DEPLOYMENTFOLDERS = datafolder

INCLUDEPATH += ../common

include(deploy.pri)
qtcAddDeployment()

OTHER_FILES += \
    data/project.xml \
    data/scr2prj.xml

DESTDIR=../plugins/msplans
