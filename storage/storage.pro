TARGET       = storage
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
CONFIG += c++11

INCLUDEPATH += ./ \
               ../../hg/hg_utility/include \
               ../../hg/hg_style \
               ../../hg/hg_display \
               ../../hg/hg_interact \
               ../../base/base/include \
               ../../infrastructure/command_registry/include \
               ../../infrastructure/command_registry_ex/include \
               ../../cvz/cvz-engine/include \
               ../../base/geogebra \
               ../../infrastructure/object_registry \
               ../../evo/storage_base/include \
               ../../boost


SOURCES     += ./*.cpp
HEADERS     += ./*.h

QT += widgets
