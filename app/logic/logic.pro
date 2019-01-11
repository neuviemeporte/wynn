TEMPLATE = lib
CONFIG += staticlib
QT       -= gui
TARGET = logic

TOPDIR = ../..
include($$TOPDIR/common.pri)

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$IXPLOG/include $$APPDIR

SOURCES += \
        backend.cpp

HEADERS += \
        backend.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
