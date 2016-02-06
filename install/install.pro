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
    TraQuera.aspr2


win32:CONFIG(release, debug|release): {
    INNO = "C:\\Program Files (x86)\\Inno Setup 5\\iscc.exe"
    ASPROTECT = "C:\\Program Files (x86)\\ASProtect 2.11 SKE\\ASProtect.exe"

    aspr.target = ..\\client\\release\\traquera-protect.exe
    aspr.commands = $$ASPROTECT -process $$replace(PWD,/,\\)\\TraQuera.aspr2
    aspr.depends = ..\\client\\release\\traquera.exe

    iss.target  = $(DESTDIR)\\$${SETUP}.exe
    iss.commands = $$INNO /O"$(DESTDIR)"  $$replace(PWD,/,\\)\\$$ISS_FILES /dOutputBaseFilename=$$SETUP
    iss.depends = $$aspr.target

    QMAKE_EXTRA_TARGETS += aspr iss
    QMAKE_EXTRA_COMPILERS +=

    OBJECTS = $$iss.target
}

