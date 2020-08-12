 
cmake_minimum_required (VERSION 2.8.11)

set(CROSS_COMPILE m68k-amigaos-)

SET(CMAKE_CXX_COMPILER m68k-amigaos-g++)

SET(CMAKE_CXX_FLAGS_DEBUG "-noixemul -Wall -O0 -g -Wl,--amiga-debug-hunk -ldebug -D LINUX")
SET(CMAKE_CXX_FLAGS_RELEASE "-noixemul -Wall -Os -s -D LINUX")


set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <OBJECTS>  -o <TARGET>")

add_executable (ADiffView
                amigautils/AmigaFile.cpp
                amigautils/MemoryPool.cpp
                amigautils/MessageBox.cpp
                amigautils/StopWatch.cpp
                application/ADiffViewArgs.cpp
                application/ADiffViewPens.cpp
                application/ADiffViewPorts.cpp
                application/ADiffViewSettings.cpp
                application/Application.cpp
                application/DiffDocument.cpp
                application/DiffWorker.cpp
                application/DiffWorkerProgressReporter.cpp
                commands/CmdCloseWindow.cpp
                commands/CmdDiff.cpp
                commands/CmdFileRequester.cpp
                commands/CmdNavNextDiff.cpp
                commands/CmdNavPrevDiff.cpp
                commands/CmdOpenWindow.cpp
                commands/CmdQuit.cpp
                commands/CmdRequester.cpp
                commands/CommandBase.cpp
                diffengine/DiffEngine.cpp
                diffengine/DiffFileBase.cpp
                diffengine/DiffInputFileAmiga.cpp
                diffengine/DiffInputFileBase.cpp
                diffengine/DiffOutputFileAmiga.cpp
                diffengine/DiffOutputFileBase.cpp
                diffengine/DiffLine.cpp
                menus/MenuBase.cpp
                menus/MenuFilesWindow.cpp
                menus/MenuDiffWindow.cpp
                model/Pair.cpp
                model/Rect.cpp
                screen/OpenClonedWorkbenchScreen.cpp
                screen/OpenJoinedPublicScreen.cpp
                screen/OpenScreenBase.cpp
                windows/DiffWindow.cpp
                windows/DiffWindowTextArea.cpp
                windows/FilesWindow.cpp
                windows/ProgressWindow.cpp
                windows/ScrollbarWindow.cpp
                windows/WindowBase.cpp
                worker/ProgressReporter.cpp
                worker/WorkerBase.cpp
                main.cpp)


target_include_directories (ADiffView PUBLIC
                            amigautils
                            application
                            commands
                            diffengine
                            menus
                            model
                            screen
                            windows
                            worker
                            .)

add_custom_command(TARGET ADiffView
                   POST_BUILD
                   COMMAND cp ADiffView ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS ADiffView)
