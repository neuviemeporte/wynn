TEMPLATE = lib
CONFIG += qt dll
TARGET = ixplog

TOPDIR = ..
include($$TOPDIR/common.pri)

DEPENDPATH += include/
INCLUDEPATH += include/
DEFINES += IXPLOG_LIB

HEADERS = \
	include/libixplog_global.h \
	include/ixplog_active.h \
	include/ixplog_inactive.h

SOURCES = src/ixplog.cpp

target.path = $$INSTALL_LIB
INSTALLS += target
