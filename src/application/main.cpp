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

#include <string>

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/libraries.h>
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


extern struct IntuitionBase* IntuitionBase;


/**
 * CLI entry point
 */
int main(int argc, char **argv)
{
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
