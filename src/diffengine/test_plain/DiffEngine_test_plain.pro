TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wconversion -Wextra -pedantic #-Weffc++

INCLUDEPATH +=  ../ \
                ../../thread \
                ../../oscottlibs

SOURCES += \
    DiffEngine_test_plain.cpp \
    ../DiffEngine.cpp \
    ../DiffFilePartition.cpp \
    ../DiffLine.cpp \
    ../../oscottlibs/LinkedList.cpp \
    ../../oscottlibs/LinkedListNode.cpp \
    ../../oscottlibs/SimpleString.cpp \
    ../DiffFilePartitionLinux.cpp \
    ../DiffTrace.cpp


HEADERS += \
    ../DiffEngine.h \
    ../DiffFilePartition.h \
    ../DiffLine.h \
    ../../oscottlibs/LinkedList.h \
    ../../oscottlibs/LinkedListNode.h \
    ../../oscottlibs/SimpleString.h \
    ../DiffFilePartitionLinux.h \
    ../../oscottlibs/Array.h \
    ../DiffTrace.h
