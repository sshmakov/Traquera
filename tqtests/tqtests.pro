#-------------------------------------------------
#
# Project created by QtCreator 2016-10-24T21:10:44
#
#-------------------------------------------------

QT       += testlib

#QT       -= gui

TARGET = tst_tqtestmain
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_tqtestmain.cpp \
    ../client/unionmodel.cpp

HEADERS += \
    ../client/unionmodel.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
