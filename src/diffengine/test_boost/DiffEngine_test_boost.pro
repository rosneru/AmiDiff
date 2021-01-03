TEMPLATE = app
CONFIG += console c++11
CONFIG += debug
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wconversion -Wextra -pedantic #-Weffc++

LIBS += -lncurses \
        -lboost_unit_test_framework \

INCLUDEPATH +=  .. \
                ../../model \
                ../../textselect \


SOURCES += \
    DiffEngine_test_boost.cpp \
    ProgressReporter.cpp \
    ../DiffEngine.cpp \
    ../DiffFileBase.cpp \
    ../DiffInputFileBase.cpp \
    ../DiffInputFileLinux.cpp \
    ../DiffOutputFileBase.cpp \
    ../DiffOutputFileLinux.cpp \
    ../DiffLine.cpp \
    ../SelectableDiffFile.cpp \
    ../../model/Pair.cpp \
    ../../textselect/TextSelection.cpp \
    ../../textselect/TextSelectionRange.cpp \
    ../../textselect/TextSelectionLine.cpp \


HEADERS += \
    ProgressReporter.h \
    ../DiffEngine.h \
    ../DiffFileBase.h \
    ../DiffInputFileBase.h \
    ../DiffInputFileLinux.h \
    ../DiffOutputFileBase.h \
    ../DiffOutputFileLinux.h \
    ../DiffLine.h \
    ../SelectableDiffFile.h \
    ../../model/Pair.h \
    ../../textselect/TextSelection.h \
    ../../textselect/TextSelectionRange.h \
    ../../textselect/TextSelectionLine.h \
