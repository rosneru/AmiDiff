 
cmake_minimum_required (VERSION 2.8.11)

set(CROSS_COMPILE m68k-amigaos-)

SET(CMAKE_CXX_COMPILER m68k-amigaos-g++)

SET(CMAKE_CXX_FLAGS_DEBUG "-noixemul -fomit-frame-pointer -fno-rtti -fno-exceptions -Wall -O0 -g -Wl,--amiga-debug-hunk -ldebug -D LINUX")
SET(CMAKE_CXX_FLAGS_RELEASE "-noixemul -fomit-frame-pointer -fno-rtti -fno-exceptions -Wall -Os -s -D LINUX")


set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <OBJECTS>  -o <TARGET>")

add_executable (ADiffView
                amigautils/AmigaFile.cpp
                amigautils/AslFileRequest.cpp
                amigautils/LinkedListAmiga.cpp
                amigautils/MessageBox.cpp
                amigautils/StopWatch.cpp
                application/ADiffViewArgs.cpp
                application/ADiffViewSettings.cpp
                application/Application.cpp
                application/main.cpp
                commands/CmdAbout.cpp
                commands/CmdDiff.cpp
                commands/CmdNavNextDiff.cpp
                commands/CmdNavPrevDiff.cpp
                commands/CmdOpenWindow.cpp
                commands/CmdQuit.cpp
                commands/Command.cpp
                diffengine/DiffEngine.cpp
                diffengine/DiffEngineAmiga.cpp
                diffengine/DiffFileAmiga.cpp
                diffengine/DiffFileBase.cpp
                diffengine/DiffLine.cpp
                diffengine/Pair.cpp
                gui/ADiffViewPens.cpp
                gui/AMenu.cpp
                gui/AScreen.cpp
                gui/DiffWindow.cpp
                gui/FilesWindow.cpp
                gui/ProgressWindow.cpp
                gui/ScrollbarWindow.cpp
                gui/WindowBase.cpp
                model/DiffDocument.cpp
                oscottlibs/LinkedList.cpp
                oscottlibs/LinkedListNode.cpp
                oscottlibs/SimpleString.cpp
                worker/DiffWorker.cpp
                worker/WorkerBase.cpp)


target_include_directories (ADiffView PUBLIC
                            amigautils
                            application
                            commands
                            diffengine
                            gui
                            model
                            oscottlibs
                            worker
                            .)

add_custom_command(TARGET ADiffView
                   POST_BUILD
                   COMMAND cp ADiffView ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS ADiffView)
