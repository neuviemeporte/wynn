TEMPLATE = app
CONFIG += qt debug

MAINDIR = ..
TOPDIR = ../..
DBDIR = $$MAINDIR/db
include($$TOPDIR/common.pri)

HEADERS += \
	wynn_test.h \
	$$DBDIR/database.h

SOURCES += \
	wynn_test.cpp \
	main.cpp \
	$$DBDIR/database.cpp \
	$$DBDIR/database_xml.cpp

DESTDIR = $$BINDIR
DEPENDPATH += $$IXPLOG/include $$IXPTEST/include $$DBDIR
INCLUDEPATH += $$IXPLOG/include $$IXPTEST/include $$DBDIR
LIBS += -L$$LIBDIR -L$$DBDIR -lixplog -lixptest -lwynndb
TARGET = test
QMAKE_RPATHDIR = $$DESTDIR
