TARGET       = storage
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
CONFIG += c++11
DESTDIR = $$PWD/debug

INCLUDEPATH += $$PWD/ \
               $$PWD/../../hg/hg_utility/include \
               $$PWD/../../hg/hg_papper \
               $$PWD/../../hg/hg_style \
               $$PWD/../../hg/hg_display \
               $$PWD/../../hg/hg_interact \
               $$PWD/../../base/base/include \
               $$PWD/../../infrastructure/command_registry/include \
               $$PWD/../../infrastructure/command_registry_ex/include \
               $$PWD/../../cvz/cvz-engine/include \
               $$PWD/../../base/geogebra \
               $$PWD/../../infrastructure/object_registry \
               $$PWD/../../evo/storage_base/include \
               $$PWD/../../boost


SOURCES     += $$PWD/*.cpp
HEADERS     += $$PWD/*.h

QT += widgets
