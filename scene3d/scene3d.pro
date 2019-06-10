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
               ../../base/base_ex/include \
               ../../base/geogebra \
               ../../base/naray

SOURCES     += ./*.cpp
HEADERS     += ./*.h

QT += widgets
QT += opengl
