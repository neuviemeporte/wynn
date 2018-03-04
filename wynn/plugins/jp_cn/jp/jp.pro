TEMPLATE = lib
CONFIG += qt plugin
QT += widgets
TARGET = wynnplugin-jp

MAINDIR = ../../..
TOPDIR = $$MAINDIR/..
APPDIR = $$MAINDIR/app
include($$TOPDIR/common.pri)

DEFINES += JP_PLUGIN_LIB
INCLUDEPATH = $$IXPLOG/include $$IXPDICT/include $$APPDIR
LIBS += -lixplog -lixpdict

FORMS += \
        ../ui/kanji_dialog.ui \
        ../ui/panel.ui \
        ../ui/radical_dialog.ui \
        ../ui/settings.ui \
        ../ui/word_dialog.ui

HEADERS += \
        ../jp-cn_base.h \
        jp_plugin_global.h \
        jp_plugin.h

SOURCES += \
        ../jp-cn_base.cpp \
        jp_plugin.cpp

target.path = $$INSTALL_LIB
INSTALLS += target
