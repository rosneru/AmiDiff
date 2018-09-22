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

  struct Screen* pAppScreen;
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
  Delay(200L);

  int screenWidth = pAppScreen->Width;
  int screenHeight = pAppScreen->Height;
  int screenBarHeight = pAppScreen->BarHeight;

  int winWidth = screenWidth / 2;
  int winHeight = screenHeight - screenBarHeight - 1;

  struct TagItem windowTags[] =
  {
    {WA_Left, 0},
    {WA_Top, screenBarHeight + 1},
    {WA_Width, winWidth},
    {WA_Height, winHeight},
    {WA_Title, (ULONG)"Left Diff Window"},
    {WA_PubScreen, (ULONG)pAppScreen},
    {TAG_DONE, NULL},
  };

  struct Window* pLeftWindow;
  pLeftWindow = OpenWindowTagList(NULL, windowTags);
  if(pLeftWindow == NULL)
  {
    CloseScreen(pAppScreen);
    closeLibs();
    return 40;
  }

  // Window successfully opened
  Delay(200L);

  // TODO Instanciating menus
  CloseWindow(pLeftWindow);

  CloseScreen(pAppScreen);
  closeLibs();
  return 0;
}

void closeLibs()
{
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}
