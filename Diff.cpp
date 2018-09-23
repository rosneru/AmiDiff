#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <clib/exec_protos.h>

#include "Application.h"

void closeLibs();

struct Library* IntuitionBase;
struct Library* DosBase;
struct Library* GadToolsBase;


int main(int argc, char **argv)
{
  IntuitionBase = OpenLibrary("intuition.library", 37);
  DosBase = OpenLibrary("dos.library", 37);
  GadToolsBase = OpenLibrary("gadtools.library", 37);

  if((!IntuitionBase) || (!DosBase) || (!GadToolsBase))
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

  Application app;
  bool bSuccess = app.Run();

  closeLibs();

  return 0;
}

void closeLibs()
{
  CloseLibrary(GadToolsBase);
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}

