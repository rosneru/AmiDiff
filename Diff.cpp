#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <clib/exec_protos.h>

#include "Application.h"

void closeLibs();

struct Library* IntuitionBase;
struct Library* DosBase;
struct Library* GadToolsBase;
struct Library* AslBase;


int main(int argc, char **argv)
{
  IntuitionBase = OpenLibrary("intuition.library", 37);
  DosBase = OpenLibrary("dos.library", 37);
  GadToolsBase = OpenLibrary("gadtools.library", 37);
  AslBase = OpenLibrary("asl.library", 37);

  if((!IntuitionBase) || (!DosBase) || (!GadToolsBase) || (!AslBase))
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

  Application app(argc, argv);
  bool bSuccess = app.Run();

  closeLibs();

  return 0;
}

void closeLibs()
{
  CloseLibrary(AslBase);
  CloseLibrary(GadToolsBase);
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}

