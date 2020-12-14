TARGET     = t2l_recog_kohonen_base
CONFIG    += staticlib
TEMPLATE   = lib
#######################################################################################
CONFIG += c++11

INCLUDEPATH += ./
INCLUDEPATH += ../../base/base/include
INCLUDEPATH += ../../base/geogebra
INCLUDEPATH += ../../base/naray

SOURCES += ./*.cpp
HEADERS += ./*.h
