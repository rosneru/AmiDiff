/**
 * AmiDiff
 *
 * This is me learning the basics of AMIGA programming.
 *
 * Maybe someday this develops into a graphical diff tool for OS2.04
 * and above.
 *
 * Development environment
 *   Amiga with OS3.9
 *   NDK for AmigaOS 3.9
 *   StormC 4
 *
 * Date of project start: 18.09.2018
 * Date of version 1.0 : -
 *
 * Author: Uwe Rosner
 */
#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <clib/exec_protos.h>

#include "Application.h"

void closeLibs();

struct Library* IntuitionBase;
struct Library* DosBase;
struct Library* GadToolsBase;
struct Library* AslBase;
struct Library* GfxBase;
struct Library* UtilityBase;
struct Library* MathIeeeDoubBasBase;
struct Library* MathIeeeDoubTransBase;


int main(int argc, char **argv)
{
  IntuitionBase = OpenLibrary("intuition.library", 39);
  DosBase = OpenLibrary("dos.library", 39);
  GadToolsBase = OpenLibrary("gadtools.library", 39);
  AslBase = OpenLibrary("asl.library", 39);
  GfxBase = OpenLibrary("graphics.library", 39);
  UtilityBase = OpenLibrary("utility.library", 39);
  MathIeeeDoubBasBase = OpenLibrary("mathieeedoubbas.library", 39);
  MathIeeeDoubTransBase = OpenLibrary("mathieeedoubtrans.library", 39);

  if((!IntuitionBase) || (!DosBase) || (!GadToolsBase) ||
     (!AslBase) || (!GfxBase) || (!UtilityBase) ||
     (!MathIeeeDoubBasBase) || (!MathIeeeDoubTransBase) )
  {
    closeLibs();
    return 20;
  }

  if((IntuitionBase->lib_Version < 39) ||
     (IntuitionBase->lib_Version < 39))
  {
    closeLibs();
    return 20;
  }

  // Create a message port for shared use with all windows
  struct MsgPort* pMsgPortAppWindows = CreateMsgPort();
  if(pMsgPortAppWindows == NULL)
  {
    closeLibs();
    return 20;
  }

  // Create the application dynamically, so we can destroy it later
  // before de Message port is destroyed.
  Application* pApp = new Application(argc, argv, pMsgPortAppWindows);
  pApp->Run();

  // Destroy app
  delete pApp;

  // Destroy message port
  DeleteMsgPort(pMsgPortAppWindows);
  closeLibs();

  return 0;
}

void closeLibs()
{
  CloseLibrary(MathIeeeDoubTransBase);
  CloseLibrary(MathIeeeDoubBasBase);
  CloseLibrary(UtilityBase);
  CloseLibrary(GfxBase);
  CloseLibrary(AslBase);
  CloseLibrary(GadToolsBase);
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}
