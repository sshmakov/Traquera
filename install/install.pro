SETUP = traquera_setup
TARGET = traquera_setup_dummy

#TEMPLATE = app
CONFIG -= qt embed_manifest_exe
CONFIG += console
QT -= core gui
QMAKE_FILETAGS += ISS_FILES SETUP
ISS_FILES = setup-win32-qt4.iss
QMAKE_LINK = echo

OTHER_FILES += \
    setup-win32-qt4.iss \
    TraQuera.aspr2 \
    RS/init.xml \
    RS/record.js \
    RS/funcs3.js \
    RS/funcs2.js \
    RS/funcs.js \
    RS/email.js \
    RS/editor-funcs.js \
    RS/editor.js \
    RS/scr.xq \
    RS/print.xq \
    RS/plain.xq \
    RS/email.xq \
    RS/edit.xq \
    RS/tracker.xml \
    RS/init.xml \
    RS/style2.css \
    RS/style.css \
    RS/editor.css \
    deploy.pri


rsfolder.source = RS
rsfolder.target = .
DEPLOYMENTFOLDERS = rsfolder

include(deploy.pri)
qtcAddDeployment()

win32:CONFIG(release, debug|release): {
    INNO = "C:\\Program Files (x86)\\Inno Setup 5\\iscc.exe"
    ASPROTECT = "C:\\Program Files (x86)\\ASProtect 32\\ASProtect.exe"
    BUILDPATH=$$replace(OUT_PWD,/,\\)\\..

    aspr.target = ..\\client\\release\\traquera-protect.exe
    aspr.commands = $$ASPROTECT -process $$replace(PWD,/,\\)\\TraQuera.aspr2
    aspr.depends = ..\\client\\release\\traquera.exe
    QMAKE_EXTRA_TARGETS += aspr

#    iss_rs.target  = $(DESTDIR)\\$${SETUP}_rs.exe
#    iss_rs.commands = $$INNO /O"$(DESTDIR)"  $$replace(PWD,/,\\)\\$$ISS_FILES /dOutputBaseFilename=$${SETUP}_rs /dVARIANT=RS /dBuildPath=$$BUILDPATH
#    iss_rs.depends = $$aspr.target
#    QMAKE_EXTRA_TARGETS += iss_rs

    iss_full.target  = $(DESTDIR)\\$${SETUP}_full.exe
    iss_full.commands = $$INNO /O"$(DESTDIR)"  $$replace(PWD,/,\\)\\$$ISS_FILES /dOutputBaseFilename=$${SETUP}_full /dVARIANT=FULL /dBuildPath=$$BUILDPATH /dProtectFile
    iss_full.depends = $$aspr.target
    QMAKE_EXTRA_TARGETS += iss_full

    iss_fullrs.target  = $(DESTDIR)\\$${SETUP}_fullrs.exe
    iss_fullrs.commands = $$INNO /O"$(DESTDIR)"  $$replace(PWD,/,\\)\\$$ISS_FILES /dOutputBaseFilename=$${SETUP}_fullrs /dVARIANT=FULLRS /dBuildPath=$$BUILDPATH
    iss_fullrs.depends = $$aspr.target
    QMAKE_EXTRA_TARGETS += iss_fullrs

    iss_jira.target  = $(DESTDIR)\\$${SETUP}.exe
    iss_jira.commands = $$INNO /O"$(DESTDIR)"  $$replace(PWD,/,\\)\\$$ISS_FILES /dOutputBaseFilename=$${SETUP} /dVARIANT=JIRA /dBuildPath=$$BUILDPATH /dProtectFile
    iss_jira.depends = $$aspr.target
    QMAKE_EXTRA_TARGETS += iss_jira

    QMAKE_EXTRA_COMPILERS +=

    OBJECTS = $$iss_rs.target  $$iss_full.target $$iss_fullrs.target $$iss_jira.target
}

