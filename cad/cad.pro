TARGET       = cad
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
CONFIG += c++11

win32 {
DEFINES += WINDOWS
}

INCLUDEPATH += ./ \
               ../../hg/hg_utility/include \
               ../../hg/hg_style \
               ../../hg/hg_display \
               ../../hg/hg_interact \
               ../../base/base/include \
               ../../base/base_ex/include \
               ../../infrastructure/command_registry \
               ../../infrastructure/command_registry_ex \
               ../../infrastructure/object_registry \
               ../../cvz/cvz-engine \
               ../../base/naray \
               ../../base/geogebra \
               ../../evo/scene3d \
               ../../evo/storage_base/include \
               ../../evo/storage \
               ../../evo/cad


SOURCES     += ./*.cpp
HEADERS     += ./*.h

QT += widgets
