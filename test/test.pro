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
    main.cpp \
    dict_test.cpp

HEADERS += \
    log.hpp
