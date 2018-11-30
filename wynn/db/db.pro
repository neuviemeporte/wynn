TEMPLATE = lib
CONFIG += qt static
TARGET = wynndb

TOPDIR = ../..
include($$TOPDIR/common.pri)

INCLUDEPATH += $$IXPLOG/include
HEADERS = database.h entry.h model.h error.h common.h
SOURCES = database.cpp entry.cpp model.cpp common.cpp

CONFIG(distributed) {
HEADERS += clocks.h event.h distributed.h
SOURCES += clocks.cpp event.cpp distributed.cpp
}
