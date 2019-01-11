TEMPLATE = lib
CONFIG += qt static
TARGET = wynndb

TOPDIR = ../..
include($$TOPDIR/common.pri)

INCLUDEPATH += $$IXPLOG/include
HEADERS = database.h entry.h model.h error.h common.h quiz.h
SOURCES = database.cpp entry.cpp model.cpp common.cpp quiz.cpp

# the abandoned implementation of the distributed database based on vector clocks, to be removed at some point
CONFIG(distributed) {
HEADERS += clocks.h event.h distributed.h
SOURCES += clocks.cpp event.cpp distributed.cpp
}
