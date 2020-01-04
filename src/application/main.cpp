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
#include <exec/types.h>
#include <exec/libraries.h>
#include <intuition/intuitionbase.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <workbench/startup.h>

#include "Application.h"
#include "MessageBox.h"
#include "SimpleString.h"

#include "ADiffViewArgs.h"

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
    return 20;
  }

  // Parse the command line or Workbench start arguments
  ADiffViewArgs arguments(argc, argv);

  // Create and run the application
  Application* app = new Application(arguments);
  bool bOk = app->Run();
  if(!bOk)
  {
    // On error display a message
    request.Show(app->ErrorMsg(), "Ok");
  }

  delete app;
  return 0;
}


/**
 * Workbench entry point.
 */
void wbmain(struct WBStartup* wb)
{
  // Call the CLI entry point with argc=0
  main(0, (char **) wb);
}
