######################################################################
# Automatically generated by qmake (2.01a) ?? 8. ??? 14:02:12 2012
######################################################################

TEMPLATE = app
TARGET = traquera
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += \
        ../common

# LIBS += -l./trktool/trktooln
CONFIG += qaxcontainer
QT += sql xml webkit xmlpatterns network script scripttools
# CODECFORSRC = windows-1251
TRANSLATIONS    = \
        lang/traquera.en.ts \
        lang/traquera.ru.ts
# CODECFORTR      = Latin1
CODECFORTR = UTF-8

# Input
FORMS += ui/tracksmain.ui ui/querypage.ui ui/filters.ui ui/scrwidg.ui \
    ui/idinput.ui \
    ui/notedialog.ui \
    ui/modifypanel.ui \
    ui/ttrecwindow.ui \
    ui/preview.ui \
    notewidget.ui \
    tqcolsdialog.ui \
    tqconnectwidget.ui \
    optionsform.ui \
    proxyoptions.ui \
    tqlogindlg.ui \
    tqprjoptdlg.ui \
    filespage.ui \
    logform.ui
#    tqqrywid.ui \
#    ui/planfilesform.ui \
#    ui/projectpage.ui

HEADERS += \
        mainwindow.h querypage.h \
        filterpage.h flowlayout.h \
	scrwidg.h \
    idinput.h \
    trkdecorator.h \
    notedialog.h \
    gsmodel.h \
    modifypanel.h \
    messager.h \
    ttfolders.h \
    unionmodel.h \
    ttdelegate.h \
    ttrecwindow.h \
    preview.h \
    ttfileiconprovider.h \
    ../common/settings.h \
    ../common/ttutils.h \
    ../common/cliputil.h \
    notewidget.h \
    scrpluginfactory.h \
    previewfactory.h \
    highlighter.h \
    projecttree.h \
    tqcolsdialog.h \
    tqconnectwidget.h \
    tqproxyrecmodel.h \
    optionsform.h \
    proxyoptions.h \
    tqhistory.h \
    tqlogindlg.h \
    mainproc.h \
    tqprjoptdlg.h \
    filespage.h \
    tqrecordviewcontroller.h \
    tqqueryviewcontroller.h \
    logform.h

#    plans.h planproxy.h \
#    planfiles.h \
#    project.h \
#    projectpage.h \
#    planfilesform.h \
#    tqpluginwidget.h \
#    tqplanswidget.h

SOURCES += database.cpp main.cpp mainwindow.cpp querypage.cpp \
        filterpage.cpp flowlayout.cpp \
	scrwidg.cpp \
    idinput.cpp \
    trkdecorator.cpp \
    notedialog.cpp \
    gsmodel.cpp \
    modifypanel.cpp \
    messager.cpp \
    ttfolders.cpp \
    unionmodel.cpp \
    ttdelegate.cpp \
    ttrecwindow.cpp \
    preview.cpp \
    ttfileiconprovider.cpp \
    ../common/ttutils.cpp \
    ../common/cliputil.cpp \
    notewidget.cpp \
    scrpluginfactory.cpp \
    previewfactory.cpp \
    highlighter.cpp \
    projecttree.cpp \
    tqcolsdialog.cpp \
    tqconnectwidget.cpp \
    tqproxyrecmodel.cpp \
    optionsform.cpp \
    proxyoptions.cpp \
    tqhistory.cpp \
    tqlogindlg.cpp \
    mainproc.cpp \
    tqprjoptdlg.cpp \
    filespage.cpp \
    tqrecordviewcontroller.cpp \
    tqqueryviewcontroller.cpp \
    logform.cpp

#    planfilesform.cpp \
#    project.cpp projectpage.cpp plans.cpp planproxy.cpp planfiles.cpp \
#    tqpluginwidget.cpp \
#    tqplanswidget.cpp

RESOURCES += resources/tracks.qrc

#win32: LIBS += -L$$PWD/../trktool/lib/ -ltrktooln -lshell32 -lshlwapi -L../lib -ltqplugapi
win32: LIBS += -lshell32 -lshlwapi -L../lib
# win32: LIBS += -lodbc32


#INCLUDEPATH += $$PWD/../trktool
#DEPENDPATH += $$PWD/../trktool

OTHER_FILES += \
    data/scr.xq \
    data/print.xq \
    data/plain.xq \
    data/email.xq \
    data/edit.xq \
    data/init.xml \
    data/tracker.xml \
    data/style.css \
    traquera-win.rc \
    baloons.ico \
    data/funcs3.js \
    data/funcs2.js \
    data/funcs.js \
    data/e-window.js \
    data/e-widget.js \
    data/email.js \
    data/e-inplace.js \
    data/editor-common.js \
    data/style2.css \
    data/editor.css \
    lang/traquera.ru.ts \
    lang/traquera.en.ts


datafolder.source = data
datafolder.target = .
langfolder.source = lang
langfolder.target = .
DEPLOYMENTFOLDERS = datafolder langfolder

include(deploy.pri)
qtcAddDeployment()

DEFINES -= CONSOLE_APP
DEFINES += CLIENT_APP

RC_FILE = traquera-win.rc

INCLUDEPATH += ../tqaxfactory
LIBS += -ltqaxfactory

#INCLUDEPATH += /Projects/ImageMagick-6.8.7-Q16/include
#LIBS += -L/Projects/ImageMagick-6.8.7-Q16/lib -lCORE_RL_Magick++_
win32:INCLUDEPATH += C:/OpenSSL-Win32/include
#win32:LIBS += -LC:/OpenSSL-Win32/lib -LC:/OpenSSL-Win32/bin -llibeay32 -lssleay32
win32:LIBS += -LC:/OpenSSL-Win32/bin
LIBS += -L../lib -ltqplugapi -ltqcondapi


