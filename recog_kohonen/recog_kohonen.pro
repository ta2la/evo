TARGET     = t2l_recog_kohonen
CONFIG    += staticlib
TEMPLATE   = lib
#######################################################################################
CONFIG += c++11

INCLUDEPATH += ./
INCLUDEPATH += ../../base/base/include
INCLUDEPATH += ../../base/base_ex/include
INCLUDEPATH += ../../base/geogebra
INCLUDEPATH += ../../base/naray
INCLUDEPATH += ../../infrastructure/command_registry
INCLUDEPATH += ../../infrastructure/command_registry_ex
INCLUDEPATH += ../../infrastructure/object_registry
INCLUDEPATH += ../../infrastructure/html_view
INCLUDEPATH += ../../hg/hg_utility/include
INCLUDEPATH += ../../hg/hg_style
INCLUDEPATH += ../../hg/hg_display
INCLUDEPATH += ../../hg/hg_interact
INCLUDEPATH += ../../evo/storage
INCLUDEPATH += ../../evo/storage_base/include
INCLUDEPATH += ../../evo/recog
INCLUDEPATH += ../../evo/storage_base
INCLUDEPATH += ../../evo/storage
INCLUDEPATH += ../../evo/cad
INCLUDEPATH += ../../evo/cad_ex
INCLUDEPATH += ../../evo/recog_kohonen_base

SOURCES += ./*.cpp
HEADERS += ./*.h

QT += widgets
