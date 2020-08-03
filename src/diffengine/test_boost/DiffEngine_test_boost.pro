TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wconversion -Wextra -pedantic #-Weffc++

LIBS += -lncurses \
        -lboost_unit_test_framework \

INCLUDEPATH +=  ../ 


SOURCES += \
    DiffEngine_test_boost.cpp \
    ../DiffEngine.cpp \
    ../DiffFileBase.cpp \
    ../DiffInputFileBase.cpp \
    ../DiffInputFileLinux.cpp \
    ../DiffLine.cpp \
    ../Pair.cpp 


HEADERS += \
    ProgressReporter.h \
    ../DiffEngine.h \
    ../DiffFileBase.h \
    ../DiffInputFileBase.h \
    ../DiffInputFileLinux.h \
    ../DiffLine.h \
    ../Pair.h 
