TARGET       = storage_base
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
wasm {
    CONFIG(debug, debug|release): SOURCEDIR = debug_wasm
    else:                         SOURCEDIR = release_wasm
} else {
    CONFIG(debug, debug|release): SOURCEDIR = debug
    else:                         SOURCEDIR = release
}
DESTDIR = $$PWD/../../BUILD/$$TARGET/$$SOURCEDIR

INCLUDEPATH += $$PWD/include \
               $$PWD/../../base/geogebra \
               $$PWD/../../base/base/include

SOURCES     += $$PWD/src/*.cpp
HEADERS     += $$PWD/include/*.h
