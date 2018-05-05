TEMPLATE = app
QT += core
QT -= gui
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle
TARGET = test

TOPDIR = ..
include($$TOPDIR/common.pri)

SOURCES += main.cpp hanchar_test.cpp
INCLUDEPATH += $$IXPDICT/include 
DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -lgtest -lixplog -lixpdict
QMAKE_CXXFLAGS += -pthread
QMAKE_RPATHDIR = $$INSTALL_LIB .
