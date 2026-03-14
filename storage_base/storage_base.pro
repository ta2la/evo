TARGET       = storage_base
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
DESTDIR = $$PWD/debug

INCLUDEPATH += $$PWD/include \
               $$PWD/../../base/geogebra \
               $$PWD/../../base/base/include

SOURCES     += $$PWD/src/*.cpp
HEADERS     += $$PWD/include/*.h
