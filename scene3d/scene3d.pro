TARGET       = scene3d
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
CONFIG += c++11

win32 {
DEFINES += WINDOWS
}

INCLUDEPATH += ./ \
               ../../base/base/include \
               ../../base/geogebra \
               ../../hg/hg_utility/include \
               ../../hg/hg_style \
               ../../hg/hg_display \
               ../../hg/hg_interact \
               ../../infrastructure/object_registry \
               ../../evo/cad \
               ../../evo/storage \
               ../../infrastructure/command_registry \
               ../../infrastructure/command_registry_ex

SOURCES     += ./*.cpp
HEADERS     += ./*.h

QT += widgets
QT += opengl
