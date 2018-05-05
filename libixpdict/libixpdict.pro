TEMPLATE = lib
CONFIG += qt dll
TARGET = ixpdict

TOPDIR = ..
include($$TOPDIR/common.pri)

INCLUDEPATH = include/ $$IXPLOG/include
DEFINES += IXPDICT_LIB
LIBS += -lixplog

HEADERS = \
	include/ixpdict_global.h \
	include/ixpdict.h \
	include/dictionary_store.h \
	include/util.h \
	include/hanchar.h \
	include/japanese.h \
	include/mandarin.h \
	include/german.h

SOURCES = \
	src/ixpdict.cpp \
	src/util.cpp \
	src/hanchar.cpp \
	src/japanese.cpp \
	src/mandarin.cpp \
	src/german.cpp

target.path = $$INSTALL_LIB
INSTALLS += target
QMAKE_RPATHDIR = $$INSTALL_LIB .