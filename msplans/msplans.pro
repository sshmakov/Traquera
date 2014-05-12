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
 project.cpp

HEADERS += plans.h\
 msplans_global.h    \
 planfiles.h         \
 planfilesform.h     \
 planproxy.h         \
 tqpluginwidget.h    \
 tqplanswidget.h     \
 project.h


FORMS = \
  planfilesform.ui \
  prjfiledlg.ui

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
