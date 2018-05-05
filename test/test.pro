TEMPLATE = app
QT += core
QT -= gui
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle
TARGET = test

TOPDIR = ..
include($$TOPDIR/common.pri)

SOURCES += main.cpp
DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -lgtest
QMAKE_CXXFLAGS += -pthread

