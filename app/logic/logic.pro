TEMPLATE = lib
CONFIG += staticlib
# TODO: get rid of this, needed for plugins
QT += gui widgets 
TARGET = logic

TOPDIR = ../..
include($$TOPDIR/common.pri)

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$IXPLOG/include $$APPDIR $$PLUGINS

SOURCES += backend.cpp \
        ext_backend.cpp \
        dict_table.cpp

HEADERS += $$PLUGINS/dict_plugin.h \
        backend.h \
        ext_backend.h \
        dict_table.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
