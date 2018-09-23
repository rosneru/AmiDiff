#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

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

  struct NewMenu appMenuDefinition[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {  NM_ITEM,   "Open left file...",     "L", 0, 0, 0 },
    {  NM_ITEM,   "Open right file...",    "R", 0, 0, 0 },
    {  NM_ITEM,   NM_BARLABEL,              0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",                  "Q", 0, 0, 0 },
    {  NM_END,    NULL,                     0 , 0, 0, 0 },
  };

  // Prepare menu: get visual info from screen
  APTR* pVisualInfo = NULL;
  pVisualInfo = (APTR*)GetVisualInfo(screen.Screen(), TAG_END);
  if(pVisualInfo == NULL)
  {
    // Getting visual info has failed
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();
    return 60;

  }

  // Menu building step 1: Create the menu
  struct Menu* pAppMenu = CreateMenus(appMenuDefinition, TAG_END);
  if(pAppMenu == NULL)
  {
    // Creating the menu has failed
    FreeVisualInfo(pVisualInfo);
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();
    return 70;
  }

  // Menu building step 2: Layout the menu
  if(LayoutMenus(pAppMenu, pVisualInfo,
                 GTMN_NewLookMenus, TRUE, // Ignored before v39
                 TAG_END) == FALSE)
  {
    FreeMenus(pAppMenu);
    FreeVisualInfo(pVisualInfo);
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();
    return 80;
  }

  // Wire the menu strip into the window
  if(SetMenuStrip(leftWindow.Window(), pAppMenu) == FALSE)
  {
    FreeMenus(pAppMenu);
    FreeVisualInfo(pVisualInfo);
    rightWindow.Close();
    leftWindow.Close();
    screen.Close();
    closeLibs();
    return 90;

  }

  handleWindowEvents(leftWindow.Window(), pAppMenu);

  ClearMenuStrip(leftWindow.Window());
  FreeMenus(pAppMenu);
  FreeVisualInfo(pVisualInfo);
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

void handleWindowEvents(struct Window *win, struct Menu *menuStrip)
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
    Wait(1L << win->UserPort->mp_SigBit);

    while ((FALSE == done) &&
            (NULL != (msg = (struct IntuiMessage *)GetMsg(win->UserPort))))
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
