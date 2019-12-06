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

#include "Application.h"
#include "MessageBox.h"
#include "SimpleString.h"

#include "ADiffViewOptions.h"


extern struct IntuitionBase* IntuitionBase;

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

  ADiffViewOptions options(argc, argv);

  // Create a message port for shared use with all windows
  struct MsgPort* pMsgPortIDCMP = CreateMsgPort();
  if(pMsgPortIDCMP == NULL)
  {
    request.Show("Error: Can't create idcmp message port.", "Ok");
    return 30;
  }

  struct MsgPort* pMsgPortProgress = CreateMsgPort();
  if(pMsgPortProgress == NULL)
  {
    request.Show("Error: Can't create progress message port.", "Ok");
    DeleteMsgPort(pMsgPortIDCMP);
    return 30;
  }

  // Create the application dynamically, so we can destroy it later
  // before the message ports are destroyed.
  Application* pApp = new Application(options, pMsgPortIDCMP, pMsgPortProgress);
  bool bOk = pApp->Run();
  if(!bOk)
  {
    // On error display a message
    request.Show(pApp->ErrorMsg(), "Ok");
  }

  // Destroy app
  delete pApp;

  // Destroy message ports
  DeleteMsgPort(pMsgPortProgress);
  DeleteMsgPort(pMsgPortIDCMP);

  return 0;
}

void wbmain(struct WBStartup* wb)
{
  main(0, (char **) wb);
}

void exctractArgs(int argc, 
                  char **argv,
                  SimpleString& pubScreenName,
                  bool& bDontAsk,
                  SimpleString& leftFilePath,
                  SimpleString& rightFilePath)
{

}
