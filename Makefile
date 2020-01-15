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
		$(SRC_ROOT)/amigautils/LinkedListAmiga.h \
		$(SRC_ROOT)/amigautils/MessageBox.h \
		$(SRC_ROOT)/amigautils/StopWatch.h \
		$(SRC_ROOT)/application/ADiffView_rev.h \
		$(SRC_ROOT)/application/ADiffViewArgs.h \
		$(SRC_ROOT)/application/ADiffViewSettings.h \
		$(SRC_ROOT)/application/Application.h \
		$(SRC_ROOT)/commands/CmdAbout.h \
		$(SRC_ROOT)/commands/CmdDiff.h \
		$(SRC_ROOT)/commands/CmdNavNextDiff.h \
		$(SRC_ROOT)/commands/CmdNavPrevDiff.h \
		$(SRC_ROOT)/commands/CmdOpenWindow.h \
		$(SRC_ROOT)/commands/CmdQuit.h \
		$(SRC_ROOT)/commands/Command.h \
		$(SRC_ROOT)/diffengine/DiffEngine.h \
		$(SRC_ROOT)/diffengine/DiffEngineAmiga.h \
		$(SRC_ROOT)/diffengine/DiffFileAmiga.h \
		$(SRC_ROOT)/diffengine/DiffFileBase.h \
		$(SRC_ROOT)/diffengine/DiffLine.h \
		$(SRC_ROOT)/diffengine/Pair.h \
		$(SRC_ROOT)/gui/ADiffViewPens.h \
		$(SRC_ROOT)/gui/AMenu.h \
		$(SRC_ROOT)/gui/AScreen.h \
		$(SRC_ROOT)/gui/DiffWindow.h \
		$(SRC_ROOT)/gui/FilesWindow.h \
		$(SRC_ROOT)/gui/ProgressWindow.h \
		$(SRC_ROOT)/gui/ScrollbarWindow.h \
		$(SRC_ROOT)/gui/WindowBase.h \
		$(SRC_ROOT)/model/DiffDocument.h \
		$(SRC_ROOT)/oscottlibs/LinkedList.h \
		$(SRC_ROOT)/oscottlibs/LinkedListNode.h \
		$(SRC_ROOT)/oscottlibs/SimpleString.h \
		$(SRC_ROOT)/worker/DiffWorker.h \
		$(SRC_ROOT)/worker/ProgressMessage.h \
		$(SRC_ROOT)/worker/ProgressReporter.h \
		$(SRC_ROOT)/worker/WorkerBase.h \
		$(SRC_ROOT)/worker/WorkerStartupMsg.h

_OBJ=$(SRC_ROOT)/amigautils/AmigaFile.o \
		$(SRC_ROOT)/amigautils/AslFileRequest.o \
		$(SRC_ROOT)/amigautils/LinkedListAmiga.o \
		$(SRC_ROOT)/amigautils/MessageBox.o \
		$(SRC_ROOT)/amigautils/StopWatch.o \
		$(SRC_ROOT)/application/ADiffViewArgs.o \
		$(SRC_ROOT)/application/ADiffViewSettings.o \
		$(SRC_ROOT)/application/Application.o \
		$(SRC_ROOT)/application/main.o \
		$(SRC_ROOT)/commands/CmdAbout.o \
		$(SRC_ROOT)/commands/CmdDiff.o \
		$(SRC_ROOT)/commands/CmdNavNextDiff.o \
		$(SRC_ROOT)/commands/CmdNavPrevDiff.o \
		$(SRC_ROOT)/commands/CmdOpenWindow.o \
		$(SRC_ROOT)/commands/CmdQuit.o \
		$(SRC_ROOT)/commands/Command.o \
		$(SRC_ROOT)/diffengine/DiffEngine.o \
		$(SRC_ROOT)/diffengine/DiffEngineAmiga.o \
		$(SRC_ROOT)/diffengine/DiffFileAmiga.o \
		$(SRC_ROOT)/diffengine/DiffFileBase.o \
		$(SRC_ROOT)/diffengine/DiffLine.o \
		$(SRC_ROOT)/diffengine/Pair.o \
		$(SRC_ROOT)/gui/ADiffViewPens.o \
		$(SRC_ROOT)/gui/AMenu.o \
		$(SRC_ROOT)/gui/AScreen.o \
		$(SRC_ROOT)/gui/DiffWindow.o \
		$(SRC_ROOT)/gui/FilesWindow.o \
		$(SRC_ROOT)/gui/ProgressWindow.o \
		$(SRC_ROOT)/gui/ScrollbarWindow.o \
		$(SRC_ROOT)/gui/WindowBase.o \
		$(SRC_ROOT)/model/DiffDocument.o \
		$(SRC_ROOT)/oscottlibs/LinkedList.o \
		$(SRC_ROOT)/oscottlibs/LinkedListNode.o \
		$(SRC_ROOT)/oscottlibs/SimpleString.o \
		$(SRC_ROOT)/worker/DiffWorker.o \
		$(SRC_ROOT)/worker/WorkerBase.o
 
INCLUDE=-I$(SRC_ROOT) \
				-I$(SRC_ROOT)/amigautils \
				-I$(SRC_ROOT)/application \
				-I$(SRC_ROOT)/commands \
				-I$(SRC_ROOT)/diffengine \
				-I$(SRC_ROOT)/gui \
				-I$(SRC_ROOT)/model \
				-I$(SRC_ROOT)/oscottlibs \
				-I$(SRC_ROOT)/worker

# Setting up compiler, flags and tools
CXX=/opt/amiga/bin/m68k-amigaos-c++
CXXFLAGS=-Wall -Wno-unused-function -fomit-frame-pointer -fno-rtti -fno-exceptions -noixemul -D LINUX

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CXXFLAGS += -O0 -g
else
# "Release" build - optimization and no debug symbols
CXXFLAGS += -Os
endif

STRIP=/opt/amiga/bin/m68k-amigaos-strip

# Replace prefix SRC_ROOT with OBJ_DIR for object file target
OBJ=$(patsubst $(SRC_ROOT)/%,$(OBJ_DIR)/%,$(_OBJ))

# Building the .o files
# Note: the mkdir line creates the needed subdirectories in OB_DIR 
$(OBJ_DIR)/%.o: $(SRC_ROOT)/%.cpp $(DEPS)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $< 

# Building the executable
$(APPNAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

# In the release build also strip the executable
ifneq ($(BUILD),debug)
	$(STRIP) $@
endif

# Cleaning build directory and executable
clean:
	rm -rf $(OBJ_DIR) $(APPNAME)

debug:
	make "BUILD=debug"
