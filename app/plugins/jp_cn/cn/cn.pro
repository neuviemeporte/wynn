TEMPLATE = lib
CONFIG += qt plugin
QT += widgets
TARGET = wynnplugin-cn

MAINDIR = ../../..
TOPDIR = $$MAINDIR/..
include($$TOPDIR/common.pri)

DEFINES += CN_PLUGIN_LIB
INCLUDEPATH = $$IXPLOG/include $$IXPDICT/include $$PLUGINS
LIBS += -lixplog -lixpdict

FORMS += \
	../ui/kanji_dialog.ui \
	../ui/panel.ui \
	../ui/radical_dialog.ui \
	../ui/settings.ui \
	../ui/word_dialog.ui

HEADERS += \
	../jp-cn_base.h \
	cn_plugin_global.h \
	cn_plugin.h

SOURCES += \
	../jp-cn_base.cpp \
	cn_plugin.cpp

target.path = $$INSTALL_LIB
INSTALLS += target
QMAKE_RPATHDIR = $$INSTALL_LIB .