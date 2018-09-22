#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>

void closeLibs();

struct Library* IntuitionBase;
struct Library* DosBase;

int main(int argc, char **argv)
{
  struct Screen* pAppScreen;

  IntuitionBase = OpenLibrary("intuition.library", 37);
  DosBase = OpenLibrary("dos.library", 37);

  if((!IntuitionBase) || (!DosBase))
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

      pAppScreen = OpenScreenTags(NULL,
                    SA_LikeWorkbench, TRUE,
                    SA_Title, "AmiDiff (C) 2018 by Uwe Rosner.",
                    TAG_DONE);

      if (pAppScreen == NULL)
      {
        closeLibs();
        return 30;
      }

      // Screen successfully opened
      Delay(300L);

      // TODO Opening Windows etc

      CloseScreen(pAppScreen);
      closeLibs();
      return 0;
}

void closeLibs()
{
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}
