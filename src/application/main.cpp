/**
 * ADiffView
 *
 * A simple graphical viewer for Differences in ASCII text files.
 *
 * Running on AMIGA computers with at least OS 3.0 / v39.
 *
 *
 * Development environment
 *   (1) DEVELOPING, TESTING, DEBUGGING, BUILD
 *       Amiga with OS 3.1.4 on WinUAE
 *       NDK for AmigaOS 3.1
 *       StormC 4
 *
 *   (2) CROSS DEVELOPING
 *       Linux / Debian Stretch in a VM
 *       Amiga OS toolchain from https://github.com/bebbo
 *       Visual Studio Code
 *
 *   (3) TESTING SOME SUBMODULES
 *       Linux / Debian Stretch in a VM
 *       UnitTests with QTCreator and boost framework
 *
 * History
 *   01.01.2020 - Release of version 1.2
 *   22.12.2019 - Release of version 1.1
 *   13.03.2019 - Release of version 1.0.1
 *   21.02.2019 - Release of version 1.0.0
 *   18.09.2018 - Project start
 *
 * Author: Uwe Rosner
 *    https://github.com/rosneru
 */


#define INTUI_V36_NAMES_ONLY

#include <stdio.h>
#include <stdlib.h>

#include <dos/dos.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <exec/types.h>
#include <intuition/intuitionbase.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <workbench/startup.h>

#include "ADiffViewArgs.h"
#include "ADiffViewSettings.h"
#include "ADiffView_rev.h"
#include "Application.h"
#include "OpenClonedWorkbenchScreen.h"
#include "OpenJoinedPublicScreen.h"
#include "MessageBox.h"
#include "OpenScreenBase.h"

#define STACKSIZE_NEEDED 8192

extern struct IntuitionBase* IntuitionBase;

/**
 * StackSwap data. Must be in global scope.
 */
struct StackSwapStruct MyStackSwap;

/**
 * Return of 'proper' main function main2. Must be in global scope.
 */
int MainReturn;

/**
 * Declaration of the 'proper' main function. It is called from main
 * after that has done its work of increasing the stack if necessary.
 */
int main2(int argc, char **argv);

/**
 * The main entry point. Only increases the stack size (if necessary)
 * and then calls the proper main() function.
 */
int main(int argc, char **argv)
{
  struct Task* pTask = FindTask(NULL);
  ULONG stackSize = (ULONG) pTask->tc_SPUpper - (ULONG) pTask->tc_SPLower;

  printf("Current stack: %lu\n", stackSize);

  if(stackSize < STACKSIZE_NEEDED)
  {
    ULONG* myStack = (ULONG*) AllocVec(STACKSIZE_NEEDED, MEMF_PUBLIC);
    if(myStack == NULL)
    {
      printf("Failed to allocate new stack.\n");
      return RETURN_FAIL;
    }

    MyStackSwap.stk_Lower = myStack;
    MyStackSwap.stk_Upper = (ULONG) myStack + STACKSIZE_NEEDED;

    MyStackSwap.stk_Pointer = (void*)(MyStackSwap.stk_Upper - 16);
    StackSwap(&MyStackSwap);

    // From here until the second StackSwap() call, the local variables
    // within this function are off-limits (unless your compiler
    // accesses them through a register other than A7 with a
    // stack-frame)

    MainReturn = main2(argc, argv);

    StackSwap(&MyStackSwap);
    
    // Now locals can be accessed again
    FreeVec(myStack);

    return MainReturn;
  }
  else
  {
    return main2(argc, argv);
  }
}

/**
 * CLI entry point
 */
int main2(int argc, char **argv)
{
  struct Task* pTask = FindTask(NULL);
  ULONG stackSize = (ULONG) pTask->tc_SPUpper - (ULONG) pTask->tc_SPLower;

  printf("New stack: %lu\n", stackSize);

  MessageBox request;

  // Check if the OS version is at least v39 / OS 3.0; return otherwise
  if(IntuitionBase->LibNode.lib_Version < 39)
  {
    request.Show("This program needs at least OS 3.0 / v39 to run.",
                 "Ok");
    return RETURN_FAIL;
  }


  ULONG exitCode = RETURN_OK;

  try
  {
    // Parse the command line or Workbench start arguments
    ADiffViewArgs args(argc, argv);

    // Create a settings instance
    ADiffViewSettings settings;

    // Create (and open) the screen depending on args
    if(args.PubScreenName().length() > 0)
    {
      // Run the app on a given public screen
      OpenJoinedPublicScreen screen(settings, args.PubScreenName().c_str());
      Application app(screen, args, settings);
      app.Run();
    }
    else
    {
      // Run the app on a Workbench screen clone with 8 colors
      OpenClonedWorkbenchScreen screen(settings, VERS, 3);
      Application app(screen, args, settings);
      app.Run();
    }

    // Create and run the application
  }
  catch(const char* pMsg)
  {
    std::string msgString = pMsg;
    request.Show("ADiffView internal error",
                 pMsg,
                 "Exit");

    exitCode = RETURN_FAIL;
  }

  return exitCode;
}


/**
 * Workbench entry point.
 */
void wbmain(struct WBStartup* wb)
{
  // Call the CLI entry point with argc=0
  main(0, (char **) wb);
}
