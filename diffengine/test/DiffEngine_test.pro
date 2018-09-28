TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wconversion -Wextra -pedantic #-Weffc++

LIBS += -lncurses \
        -lboost_unit_test_framework \

INCLUDEPATH +=  ../ \
                ../../oscottlibs

SOURCES += \
    OscottModels_test.cpp \
    ../TextDocument.cpp \
    ../../oscottlibs/DiffEngine.cpp \
    ../../oscottlibs/FilePartition.cpp \
    ../../oscottlibs/LinkedListNode.cpp \
    ../../oscottlibs/SimpleString.cpp


HEADERS += \
    ../TextDocument.h \
    ../../oscottlibs/DiffEngine.h \
    ../../oscottlibs/FilePartition.h \
    ../../oscottlibs/LinkedListNode.h \
    ../../oscottlibs/SimpleString.h
