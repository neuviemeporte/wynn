TEMPLATE = app
CONFIG += qt
TARGET = dicttool

TOPDIR = ..
include($$TOPDIR/common.pri)

INCLUDEPATH += $$IXPLOG/include $$IXPDICT/include
LIBS += -lixplog -lixpdict

HEADERS += dicttool.h
SOURCES += dicttool.cpp main.cpp

QMAKE_RPATHDIR = $$DESTDIR
