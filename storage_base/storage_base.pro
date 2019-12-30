TARGET       = storage_base
TEMPLATE     = lib
CONFIG      += staticlib
#######################################################################################
INCLUDEPATH += ./include \
               ../../base/geogebra \
               ../../base/base/include

SOURCES     += ./src/*.cpp
HEADERS     += ./include/*.h
