debug {
    message($$TARGET debug build)
    BUILD_DIR = $$TOPDIR/build-debug
    DEFINES += DEBUG
}
else {
    message($$TARGET release build)
    BUILD_DIR = $$TOPDIR/build-release
}

DESTDIR = $$BUILD_DIR
LIBS += -L$$BUILD_DIR

IXPLOG = $$TOPDIR/libixplog
IXPDICT = $$TOPDIR/libixpdict
IXPTEST = $$TOPDIR/libixptest
DATABASE = $$TOPDIR/app/db
PLUGINS = $$TOPDIR/app/plugins

unix:isEmpty(PREFIX) {
	PREFIX = /usr/local
}

INSTALL_BIN = $$PREFIX/bin
INSTALL_LIB = $$PREFIX/lib

