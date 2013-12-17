######################################################################
# Automatically generated by qmake (2.01a) ?? 8. ??? 14:02:12 2012
######################################################################

TEMPLATE = app
TARGET = traquera
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += \
	../trktool/lib \
        ../common \
        ../tracker
# LIBS += -l./trktool/trktooln
CONFIG += qaxcontainer
QT += sql xml webkit xmlpatterns network
# CODECFORSRC = windows-1251
TRANSLATIONS    = \
	lang/client_en.ts \
	lang/client_ru.ts
# CODECFORTR      = Latin1
CODECFORTR = windows-1251

# Input
FORMS += ui/tracksmain.ui ui/querypage.ui ui/projectpage.ui ui/trklogin.ui ui/filters.ui ui/scrwidg.ui \
    ui/idinput.ui \
    ui/notedialog.ui \
    ui/modifypanel.ui \
    ui/ttrecwindow.ui \
    ui/preview.ui \
    ui/planfilesform.ui \
    notewidget.ui

HEADERS += \
	mainwindow.h project.h querypage.h projectpage.h trklogin.h \
        filterpage.h flowlayout.h plans.h planproxy.h \
	scrwidg.h \
    planfiles.h \
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
    planfilesform.h \
    ../tracker/trkview.h \
    ../tracker/trktool2.h \
    ../tracker/trdefs.h \
    ../tracker/tracker.h \
    ../common/settings.h \
    ../common/ttutils.h \
    ../common/ttglobal.h \
    ../common/cliputil.h \
    notewidget.h \
    scrpluginfactory.h \
    previewfactory.h \
    highlighter.h
SOURCES += database.cpp main.cpp mainwindow.cpp project.cpp querypage.cpp projectpage.cpp \
	trklogin.cpp filterpage.cpp flowlayout.cpp plans.cpp planproxy.cpp \
	scrwidg.cpp \
    planfiles.cpp \
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
    planfilesform.cpp \
    ../common/ttutils.cpp \
    ../common/ttglobal.cpp \
    ../tracker/trkview.cpp \
    ../tracker/tracker.cpp \
    ../common/cliputil.cpp \
    notewidget.cpp \
    scrpluginfactory.cpp \
    previewfactory.cpp \
    highlighter.cpp
RESOURCES += resources/tracks.qrc

win32: LIBS += -L$$PWD/../trktool/lib/ -ltrktooln -lshell32 -lshlwapi

INCLUDEPATH += $$PWD/../trktool
DEPENDPATH += $$PWD/../trktool


OTHER_FILES += \
    data/scr.xq \
    data/print.xq \
    data/plain.xq \
    data/email.xq \
    data/edit.xq \
    data/scr2prj.xml \
    data/project.xml \
    data/init.xml \
    data/tracker.xml \
    data/style.css \
    traquera-win.rc \
    lang/tracks_ru.ts \
    lang/tracks_en.ts \
    baloons.ico


datafolder.source = data
datafolder.target = .
langfolder.source = lang
langfolder.target = .
DEPLOYMENTFOLDERS = datafolder langfolder

include(deploy.pri)
qtcAddDeployment()

DEFINES -= CONSOLE_APP

RC_FILE = traquera-win.rc
