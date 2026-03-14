TARGET       = cad_attrs
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
CONFIG += c++11
DESTDIR = $$PWD/debug

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
