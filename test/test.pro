include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt
TARGET = test

TOPDIR = ..
include($$TOPDIR/common.pri)

INCLUDEPATH += $$IXPLOG/include $$IXPDICT/include $$WYNNDB
LIBS += -lixpdict -lwynndb -lixplog 

SOURCES += \
    dict_test.cpp \
    hanchar_test.cpp \
    db_test.cpp \
    main.cpp

HEADERS += \
    log.hpp

QMAKE_RPATHDIR = $$DESTDIR