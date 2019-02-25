# Makefile for building ADiffView for m68k-AmigaOS on Linux using 
# gcc 6.5 from this toolchain: https://github.com/bebbo/amiga-gcc
#
# Related forum thread: http://eab.abime.net/showthread.php?t=85474
# 
#
# Author: Uwe Rosner
# Date: 25.02.2019
#

# Setting up application name and directories
APPNAME=ADiffView
SRC_ROOT=src
OBJ_DIR=obj

DEPS=$(SRC_ROOT)/amigautils/AmigaFile.h \
		$(SRC_ROOT)/amigautils/AslFileRequest.h \
		$(SRC_ROOT)/amigautils/MessageBox.h \
		$(SRC_ROOT)/amigautils/StopWatch.h \
		$(SRC_ROOT)/application/Application.h \
		$(SRC_ROOT)/application/DiffWorker.h \
		$(SRC_ROOT)/commands/CmdAbout.h \
		$(SRC_ROOT)/commands/CmdDiff.h \
		$(SRC_ROOT)/commands/CmdOpenWindow.h \
		$(SRC_ROOT)/commands/CmdQuit.h \
		$(SRC_ROOT)/commands/Command.h \
		$(SRC_ROOT)/diffengine/DiffEngine.h \
		$(SRC_ROOT)/diffengine/DiffFilePartition.h \
		$(SRC_ROOT)/diffengine/DiffFilePartitionAmiga.h \
		$(SRC_ROOT)/diffengine/DiffLine.h \
		$(SRC_ROOT)/gui/AmigaDiffPens.h \
		$(SRC_ROOT)/gui/ApplicationMenu.h \
		$(SRC_ROOT)/gui/AppScreen.h \
		$(SRC_ROOT)/gui/DiffWindow.h \
		$(SRC_ROOT)/gui/FilesWindow.h \
		$(SRC_ROOT)/gui/ProgressWindow.h \
		$(SRC_ROOT)/gui/ScrollbarWindow.h \
		$(SRC_ROOT)/gui/TextWindow.h \
		$(SRC_ROOT)/gui/WindowBase.h \
		$(SRC_ROOT)/model/DiffDocument.h \
		$(SRC_ROOT)/model/Document.h \
		$(SRC_ROOT)/model/TextDocument.h \
		$(SRC_ROOT)/oscottlibs/Array.h \
		$(SRC_ROOT)/oscottlibs/SimpleString.h \
		$(SRC_ROOT)/thread/BackgroundWorker.h \
		$(SRC_ROOT)/thread/ProgressReporter.h \
		$(SRC_ROOT)/thread/WorkerProgressMsg.h \
		$(SRC_ROOT)/thread/WorkerStartupMsg.h

_OBJ=$(SRC_ROOT)/main.o \
		$(SRC_ROOT)/amigautils/AmigaFile.cpp \
		$(SRC_ROOT)/amigautils/AslFileRequest.cpp \
		$(SRC_ROOT)/amigautils/MessageBox.cpp \
		$(SRC_ROOT)/amigautils/StopWatch.cpp \
		$(SRC_ROOT)/application/Application.cpp \
		$(SRC_ROOT)/application/DiffWorker.cpp \
		$(SRC_ROOT)/commands/CmdAbout.cpp \
		$(SRC_ROOT)/commands/CmdDiff.cpp \
		$(SRC_ROOT)/commands/CmdOpenWindow.cpp \
		$(SRC_ROOT)/commands/CmdQuit.cpp \
		$(SRC_ROOT)/commands/Command.cpp \
		$(SRC_ROOT)/diffengine/DiffEngine.cpp \
		$(SRC_ROOT)/diffengine/DiffFilePartition.cpp \
		$(SRC_ROOT)/diffengine/DiffFilePartitionAmiga.cpp \
		$(SRC_ROOT)/diffengine/DiffLine.cpp \
		$(SRC_ROOT)/gui/AmigaDiffPens.cpp \
		$(SRC_ROOT)/gui/ApplicationMenu.cpp \
		$(SRC_ROOT)/gui/AppScreen.cpp \
		$(SRC_ROOT)/gui/DiffWindow.cpp \
		$(SRC_ROOT)/gui/FilesWindow.cpp \
		$(SRC_ROOT)/gui/ProgressWindow.cpp \
		$(SRC_ROOT)/gui/ScrollbarWindow.cpp \
		$(SRC_ROOT)/gui/TextWindow.cpp \
		$(SRC_ROOT)/gui/WindowBase.cpp \
		$(SRC_ROOT)/model/DiffDocument.cpp \
		$(SRC_ROOT)/model/Document.cpp \
		$(SRC_ROOT)/model/TextDocument.cpp \
		$(SRC_ROOT)/oscottlibs/SimpleString.cpp \
		$(SRC_ROOT)/thread/BackgroundWorker.cpp
 
INCLUDE=-I$(SRC_ROOT) \
				-I$(SRC_ROOT)/amigautils \
				-I$(SRC_ROOT)/application \
				-I$(SRC_ROOT)/commands \
				-I$(SRC_ROOT)/diffengine \
				-I$(SRC_ROOT)/gui \
				-I$(SRC_ROOT)/model \
				-I$(SRC_ROOT)/oscottlibs \
				-I$(SRC_ROOT)/thread

# Setting up compiler, flags and tools
CXX=/opt/amiga/bin/m68k-amigaos-c++
CXXFLAGS=-Wall -Wno-unused-function -fomit-frame-pointer -fno-rtti -fno-exceptions -noixemul -Os

STRIP=/opt/amiga/bin/m68k-amigaos-strip

# Replace prefix SRC_ROOT with OBJ_DIR for object file target
OBJ=$(patsubst $(SRC_ROOT)/%,$(OBJ_DIR)/%,$(_OBJ))

# Building the .o files
# Note: the mkdir line creates the needed subdirectories in OB_DIR 
$(OBJ_DIR)/%.o: $(SRC_ROOT)/%.cpp $(DEPS)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $< 

# Building the executable
# Note: After the build the debug informations are removed from the 
#       executable using strip.
$(APPNAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^
	$(STRIP) $@

# Cleaning build directory and executable
clean:
	rm -rf $(OBJ_DIR) $(APPNAME)

