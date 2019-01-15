TEMPLATE = lib
CONFIG += qt plugin
QT += widgets
TARGET = wynnplugin-de

TOPDIR = ../../..
include($$TOPDIR/common.pri)

INCLUDEPATH = $$IXPLOG/include $$IXPDICT/include $$PLUGINS
DEFINES += DE_PLUGIN_LIB
LIBS += -lixplog -lixpdict

FORMS = details.ui panel.ui

HEADERS += $$PLUGINS/dict_plugin.h \
    de_plugin_global.h \
    de_plugin.h 

SOURCES += de_plugin.cpp

target.path = $$INSTALL_LIB
INSTALLS += target
QMAKE_RPATHDIR = $$INSTALL_LIB .