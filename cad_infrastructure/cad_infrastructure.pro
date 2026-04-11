TARGET       = cad_infrastructure
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
               $$PWD/../../evo/cad_attrs \
               $$PWD/../../evo/storage \
               $$PWD/../../evo/storage_base/include \
               $$PWD/../../hg/hg_interact \
               $$PWD/../../hg/hg_utility/include \
               $$PWD/../../hg/hg_papper \
               $$PWD/../../hg/hg_text \
               $$PWD/../../hg/hg_style \
               $$PWD/../../hg/hg_display \
               $$PWD/../../base/base/include \
               $$PWD/../../infrastructure/command_registry \
               $$PWD/../../infrastructure/command_registry_ex \
               $$PWD/../../infrastructure/object_registry \
               $$PWD/../../cvz/cvz-engine \
               $$PWD/../../base/geogebra \
               $$PWD/../../evo/route_draw

#$$PWD/../../evo/cad_draw \
#$$PWD/../../evo/cad \

SOURCES     += $$PWD/*.cpp
HEADERS     += $$PWD/*.h

#defines += CAD_PROFESSIONAL

QT += widgets
