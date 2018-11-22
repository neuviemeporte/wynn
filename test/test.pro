include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt
TARGET = test

TOPDIR = ..
include($$TOPDIR/common.pri)

INCLUDEPATH += $$IXPLOG/include $$IXPDICT/include
LIBS += -lixplog -lixpdict

SOURCES += \
    dict_test.cpp \
    hanchar_test.cpp \
    main.cpp

HEADERS += \
    log.hpp
