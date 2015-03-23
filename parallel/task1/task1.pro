TEMPLATE = app
CONFIG += console
CONFIG -= qt

INCLUDEPATH += /usr/include/openmpi

SOURCES += main.cpp \
    Solver.cpp \
    DVector.cpp \
    DMatrix.cpp \
    Task.cpp \
    debug.cpp

HEADERS += \
    Solver.h \
    DVector.h \
    DMatrix.h \
    Piece.h \
    Task.h \
    debug.h

