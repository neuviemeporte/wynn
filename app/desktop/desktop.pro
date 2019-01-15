TEMPLATE = app
CONFIG += qt
QT += widgets
TARGET = wynn

TOPDIR = ../..
include($$TOPDIR/common.pri)

DBDIR = ../db
DEPENDPATH += res ui
INCLUDEPATH += .. $$IXPLOG/include $$LOGIC $$PLUGINS 
LIBS += -L$$DBDIR -lwynndb -lixplog

HEADERS += global.h \
           setup.h \
	   window.h

FORMS += ui/dbase_entry.ui \
         ui/mainform.ui \
         ui/quiz.ui

SOURCES += main.cpp \
           setup.cpp \
	   window.cpp

RESOURCES += res/mainform.qrc
win32:RC_ICONS = gfx/wynn-icon.ico

target.path = $$INSTALL_BIN
INSTALLS += target
QMAKE_RPATHDIR = $$INSTALL_LIB .
