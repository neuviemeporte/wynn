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
    test_dict.cpp \
    test_hanchar.cpp \
    test_db.cpp \
    main.cpp \
    test_quiz.cpp

HEADERS += \
    log.hpp

QMAKE_RPATHDIR = $$DESTDIR
