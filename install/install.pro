TEMPLATE = app
TARGET = traquera_setup
CONFIG -= qt embed_manifest_exe
CONFIG += console
QT -= core gui
QMAKE_FILETAGS += ISS_FILES
ISS_FILES = setup-win32-qt4.iss
QMAKE_LINK = echo

CONFIG(release, debug|release): {
iss.output  = $(TARGET)
iss.commands = "C:\\Program Files (x86)\\Inno Setup 5\\iscc.exe" /O"$(DESTDIR)"  ${QMAKE_FILE_NAME} /dOutputBaseFilename=$$TARGET
iss.input = ISS_FILES

QMAKE_EXTRA_COMPILERS += iss
}

OTHER_FILES += \
    setup-win32-qt4.iss
