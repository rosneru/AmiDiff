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
 *       Amiga with OS3.9 on WinUAE
 *       NDK for AmigaOS 3.9
 *       StormC 4
 * 
 *   (2) CROSS DEVELOPING
 *       Linux / Debian Stretch in a VM
 *       Amiga OS toolchain from https://github.com/bebbo
 *       Visual Studio Code
 * 
 *
 * History
 *   xx.02.2019 - Release of version 1.0
 *   18.09.2018 - Project start
 *
 * Author: Uwe Rosner
 * https://github.com/rosneru
 */

#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <workbench/startup.h>

#include "Application.h"
#include "SimpleString.h"

/**
 * Closes all opened libraries
 */
void closeLibs();

/**
 * Extracts the parameters which could be provided optionally on 
 * program start via Workbench or CLI.
 * 
 * @param argc The argc variable from main()
 * @param argv The argv array from main.
 * 
 * @p_PubScreenName The name of the pubscreen to open the window on. 
 * Only set if the CLI argument or Workbench tooltype PUBSCREEN is set.
 * 
 * @p_LeftFilePath The file name of the left file if one was passed 
 * from Workbench or CLI.
 * 
 * @p_RightFilePath The file name of the right file if one was passed
 * from Workbench or CLI.
 */
void exctractArgs(int argc, char **argv,
  SimpleString& p_PubScreenName,
  SimpleString& p_LeftFilePath,
  SimpleString& p_RightFilePath);

struct Library* IntuitionBase;
struct Library* DosBase;
struct Library* GadToolsBase;
struct Library* AslBase;
struct Library* GfxBase;
struct Library* IconBase;
struct Library* UtilityBase;
struct Library* MathIeeeDoubBasBase;
struct Library* MathIeeeDoubTransBase;


int main(int argc, char **argv)
{
  IntuitionBase = OpenLibrary("intuition.library", 37);
  DosBase = OpenLibrary("dos.library", 37);
  GadToolsBase = OpenLibrary("gadtools.library", 37);
  AslBase = OpenLibrary("asl.library", 37);
  GfxBase = OpenLibrary("graphics.library", 37);
  IconBase = OpenLibrary("icon.library", 37);
  UtilityBase = OpenLibrary("utility.library", 37);
  MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library", 37);
  MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library", 37);

  if((!IntuitionBase) || (!DosBase) || (!GadToolsBase) ||
     (!AslBase) || (!GfxBase) || (!UtilityBase) ||
     (!MathIeeeDoubBasBase) || (!MathIeeeDoubTransBase) )
  {
    closeLibs();
    return 20;
  }

  if((IntuitionBase->lib_Version < 37) ||
     (IntuitionBase->lib_Version < 37))
  {
    closeLibs();
    return 20;
  }

  // Define some variables for values which optionally could be passed
  // as arguments to the app
  SimpleString pubScreenName;
  SimpleString leftFilePath;
  SimpleString rightFilePath;

  // Fill the variables with values if appropriate arguments are passed
  exctractArgs(argc, argv, pubScreenName, leftFilePath, rightFilePath);

  // Create a message port for shared use with all windows
  struct MsgPort* pMsgPortAppWindows = CreateMsgPort();
  if(pMsgPortAppWindows == NULL)
  {
    closeLibs();
    return 20;
  }

  // Create the application dynamically, so we can destroy it later
  // before de Message port is destroyed.
  Application* pApp = new Application(pMsgPortAppWindows, pubScreenName);
  pApp->SetLeftFilePath(leftFilePath);
  pApp->SetRightFilePath(rightFilePath);
  pApp->Run();

  // Destroy app
  delete pApp;

  // Destroy message port
  DeleteMsgPort(pMsgPortAppWindows);
  closeLibs();

  return 0;
}

void wbmain(struct WBStartup* wb)
{
  main(0, (char **) wb);
}


void closeLibs()
{
  CloseLibrary(MathIeeeDoubTransBase);
  CloseLibrary(MathIeeeDoubBasBase);
  CloseLibrary(UtilityBase);
  CloseLibrary(IconBase);  
  CloseLibrary(GfxBase);
  CloseLibrary(AslBase);
  CloseLibrary(GadToolsBase);
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}

void exctractArgs(int argc, char **argv,
  SimpleString& p_PubScreenName,
  SimpleString& p_LeftFilePath,
  SimpleString& p_RightFilePath)
{
  if(argc == 0)
  {
    //
    // Started from Workbench
    //

    int bufLen = 2048;  // TODO How to get rid of this fixed maximum?
    STRPTR pBuf = (STRPTR) AllocVec(bufLen, MEMF_FAST);
    if(pBuf != NULL)
    {
      struct WBStartup* wbStartup = (struct WBStartup*) argv;
      struct WBArg* wbArg = wbStartup->sm_ArgList;
      for(int i=0; i < wbStartup->sm_NumArgs; i++)
      {
        if((wbArg[i].wa_Lock != NULL))
        {
          if(i == 0)
          {
            // The application icon itself. Get the PUBSCREEN tooltype 
            // from it

            // TODO
          }
          else if(i < 3)
          {
            SimpleString fullPath;

            if(NameFromLock(wbArg[i].wa_Lock, pBuf, bufLen) != 0)
            {
              if(AddPart(pBuf,(STRPTR) wbArg[i].wa_Name, bufLen))
              {
                fullPath = pBuf;
              }
  
              if(i == 1)
              {
                p_LeftFilePath = fullPath;
              }
              else
              {
                p_RightFilePath = fullPath;
              }
            }
          }
          else
          {
            // We only need the filenames of the first 2 selected icons
            break;
          }
        }
      }

      FreeVec(pBuf);
    }
  }
}