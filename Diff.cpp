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
  DiffWindow leftWindow(screen.Screen());
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
  DiffWindow rightWindow(screen.Screen());
  if(!rightWindow.Open(DiffWindow::RIGHT))
  {
    // Opening the window failed
    leftWindow.Close();
    screen.Close();
    closeLibs();
    return 50;
  }

  //
  // Instanciating the menu via GadTools
  //

  AppMenu appMenu(screen.Screen());
  if(appMenu.Create() == FALSE)
  {
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();

    return 60;
  }

  if(appMenu.BindToWindow(leftWindow.Window()) == FALSE)
  {
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();

    return 70;
  }

  if(appMenu.BindToWindow(rightWindow.Window()) == FALSE)
  {
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();

    return 70;
  }


  handleWindowEvents(leftWindow.Window(), appMenu.Menu());

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

void handleWindowEvents(struct Window *win1, struct Menu *menuStrip)
{
  struct IntuiMessage *msg;
  SHORT done;
  UWORD menuNumber;
  UWORD menuNum;
  UWORD itemNum;
  UWORD subNum;
  struct MenuItem *item;

  done = FALSE;
  while (FALSE == done)
  {
    // we only have one signal bit, so we do not have to check which
    // bit broke the Wait().
    Wait(1L << win1->UserPort->mp_SigBit);

    while ((FALSE == done) &&
            (NULL != (msg = (struct IntuiMessage *)GetMsg(win1->UserPort))))
    {
      switch (msg->Class)
      {
//    case IDCMP_CLOSEWINDOW:
//        done = TRUE;
//        break;
      case IDCMP_MENUPICK:
          menuNumber = msg->Code;
          while ((menuNumber != MENUNULL) && (!done))
              {
              item = ItemAddress(menuStrip, menuNumber);

              /* process the item here! */
              menuNum = MENUNUM(menuNumber);
              itemNum = ITEMNUM(menuNumber);
              subNum  = SUBNUM(menuNumber);

              /* stop if quit is selected. */
              if ((menuNum == 0) && (itemNum == 3))
                  done = TRUE;

              menuNumber = item->NextSelect;
              }
          break;
      }

      ReplyMsg((struct Message *)msg);
    }
  }
}
