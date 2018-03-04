debug {
    message($$TARGET debug build)
    BUILD_DIR = $$TOPDIR/build-debug
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

unix:isEmpty(PREFIX) {
	PREFIX = /usr/local
}

INSTALL_BIN = $$PREFIX/bin
INSTALL_LIB = $$PREFIX/lib

