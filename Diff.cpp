#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

#include "AppMenu.h"
#include "AppScreen.h"
#include "DiffWindow.h"


void closeLibs();
void handleWindowEvents(struct Window *win, struct Menu *menuStrip);


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

  //
  // Opening the screen
  //

  AppScreen screen;
  if (!screen.Open())
  {
    // Opening the screen failed
    closeLibs();
    return 30;
  }


  //
  // Opening the left window
  //
  DiffWindow leftWindow(screen.IntuiScreen());
  if(!leftWindow.Open(DiffWindow::LEFT))
  {
    // Opening the window failed
    screen.Close();
    closeLibs();
    return 40;
  }

  //
  // Opening the right window
  //
  DiffWindow rightWindow(screen.IntuiScreen());
  if(!rightWindow.Open(DiffWindow::RIGHT))
  {
    // Opening the window failed
    leftWindow.Close();
    screen.Close();
    closeLibs();
    return 50;
  }

  //
  // Creating the menu
  //

  AppMenu appMenu(screen.IntuiScreen());
  if(appMenu.Create() == FALSE)
  {
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();

    return 60;
  }

  //
  // Installing menu to left window
  //

  if(appMenu.BindToWindow(leftWindow.IntuiWindow()) == FALSE)
  {
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();

    return 70;
  }

  //
  // Installing menu to left window
  //

  if(appMenu.BindToWindow(rightWindow.IntuiWindow()) == FALSE)
  {
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();

    return 70;
  }

  //
  // Waiting for messages from Intuition about user activity
  //

  int leftWinSig = 1L << leftWindow.IntuiWindow()->UserPort->mp_SigBit;
  int rightWinSig = 1L << rightWindow.IntuiWindow()->UserPort->mp_SigBit;

  bool bExit = false;
  do
  {
    int signals = Wait(leftWinSig | rightWinSig);
    struct IntuiMessage* pMsg = NULL;

    do
    {
      if(signals & leftWinSig)
      {
        pMsg = (struct IntuiMessage*)
                 GetMsg(leftWindow.IntuiWindow()->UserPort);
      }
      else if(signals & rightWinSig)
      {
        pMsg = (struct IntuiMessage*)
                 GetMsg(rightWindow.IntuiWindow()->UserPort);
      }

      switch (pMsg->Class)
      {
      case IDCMP_MENUPICK:
        UWORD menuNumber = pMsg->Code;
        while ((menuNumber != MENUNULL) && (bExit == false))
        {
          struct MenuItem* pItem = ItemAddress(appMenu.IntuiMenu(),
                                     menuNumber);

          // Which item in menu and submenu was selected?
          UWORD menuNum = MENUNUM(menuNumber);
          UWORD itemNum = ITEMNUM(menuNumber);
          UWORD subNum  = SUBNUM(menuNumber);

          /* stop if quit is selected. */
          if ((menuNum == 0) && (itemNum == 3))
          {
            bExit = true;
          }

          menuNumber = pItem->NextSelect;
        }
        break;
      }

      ReplyMsg((struct Message *)pMsg);


    }
    while(bExit == false && pMsg != NULL);

  }
  while(bExit == false);

  rightWindow.Close();
  leftWindow.Close();
  screen.Close();
  closeLibs();

  return 0;
}

void closeLibs()
{
  CloseLibrary(GadToolsBase);
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);
}

