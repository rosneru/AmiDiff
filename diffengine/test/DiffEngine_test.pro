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
    DiffEngine_test.cpp \
    ../DiffEngine.cpp \
    ../FilePartition.cpp \
    ../../oscottlibs/LinkedList.cpp \
    ../../oscottlibs/LinkedListNode.cpp \
    ../../oscottlibs/SimpleString.cpp


HEADERS += \
    ../DiffEngine.h \
    ../FilePartition.h \
    ../../oscottlibs/LinkedList.h \
    ../../oscottlibs/LinkedListNode.h \
    ../../oscottlibs/SimpleString.h