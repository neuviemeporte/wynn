TEMPLATE = lib
CONFIG += qt static
TARGET = wynndb

TOPDIR = ../..
include($$TOPDIR/common.pri)

INCLUDEPATH += $$IXPLOG/include

HEADERS = database.h
SOURCES = database.cpp database_xml.cpp 
