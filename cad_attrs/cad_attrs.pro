TARGET       = cad_attrs
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
CONFIG += c++11
wasm {
    CONFIG(debug, debug|release): SOURCEDIR = debug_wasm
    else:                         SOURCEDIR = release_wasm
} else {
    CONFIG(debug, debug|release): SOURCEDIR = debug
    else:                         SOURCEDIR = release
}
DESTDIR = $$PWD/../../BUILD/$$TARGET/$$SOURCEDIR

win32 {
DEFINES += WINDOWS
}

INCLUDEPATH += $$PWD/ \
               $$PWD/../../base/base/include \
               $$PWD/../../hg/hg_style \
               $$PWD/../../evo/storage

SOURCES     += $$PWD/*.cpp
HEADERS     += $$PWD/*.h

QT += widgets
