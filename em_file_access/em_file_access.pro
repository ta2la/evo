TARGET       = em_file_access
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
                $$PWD/../../hg/hg_utility/include \
                $$PWD/../../hg/hg_papper \
                $$PWD/../../hg/hg_text \
                $$PWD/../../hg/hg_style \
                $$PWD/../../hg/hg_display \
                $$PWD/../../hg/hg_interact \
                $$PWD/../../base/base/include \
                $$PWD/../../infrastructure/object_registry \
                #$$PWD/../../infrastructure/html_view \
                $$PWD/../../cvz/cvz-engine \
                $$PWD/../../infrastructure/command_registry \
                $$PWD/../../infrastructure/command_registry_ex \
                $$PWD/../../base/geogebra \
                $$PWD/../../evo/storage_base/include \
                $$PWD/../../evo/storage \
                $$PWD/../../evo/cad_infrastructure \
                $$PWD/../../evo/cad_attrs \
                $$PWD/../../cad/cad_settings \
                $$PWD/../../evo/cad_draw \
                $$PWD/../../evo/cad \
                $$PWD/../../evo/cad_qml \
                $$PWD/../../base2/file_manager2


UNIVERSEDIR = $$PWD/../../../../data/1302_FLOW/packages/1302_datafromsky_trafficengine/app/common/universe
INCLUDEPATH += $$UNIVERSEDIR/cmd_sys

INCLUDEPATH += $$UNIVERSEDIR/base
INCLUDEPATH += $$UNIVERSEDIR/utility
INCLUDEPATH += $$UNIVERSEDIR/universe_server_shared

SOURCES     += $$PWD/*.cpp
HEADERS     += $$PWD/*.h

QT += quick widgets gui
