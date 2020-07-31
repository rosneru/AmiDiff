TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wconversion -Wextra -pedantic #-Weffc++

LIBS += -lncurses \
        -lboost_unit_test_framework \

INCLUDEPATH +=  ../ \
                ../../worker \
                ../../oscottlibs

SOURCES += \
    DiffEngine_test_boost.cpp \
    ../DiffEngine.cpp \
    ../DiffFileBase.cpp \
    ../DiffLine.cpp \
    ../../oscottlibs/LinkedList.cpp \
    ../../oscottlibs/LinkedListNode.cpp \
    ../../oscottlibs/std::string.cpp \
    ../Pair.cpp \
    ../DiffFileLinux.cpp


HEADERS += \
    ../DiffEngine.h \
    ../DiffFileBase.h \
    ../DiffLine.h \
    ../../oscottlibs/LinkedList.h \
    ../../oscottlibs/LinkedListNode.h \
    ../../oscottlibs/std::string.h \
    ../../oscottlibs/Array.h \
    ../Pair.h \
    ../DiffFileLinux.h
