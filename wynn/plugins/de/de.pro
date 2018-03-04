TEMPLATE = lib
CONFIG += qt plugin
QT += widgets
TARGET = wynnplugin-de

TOPDIR = ../../..
include($$TOPDIR/common.pri)
APPDIR = ../../app

INCLUDEPATH = $$IXPLOG/include $$IXPDICT/include $$APPDIR
DEFINES += DE_PLUGIN_LIB
LIBS += -lixplog -lixpdict

FORMS = details.ui panel.ui

HEADERS += de_plugin_global.h de_plugin.h

SOURCES += de_plugin.cpp

target.path = $$INSTALL_LIB
INSTALLS += target
