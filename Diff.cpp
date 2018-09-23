#define INTUI_V36_NAMES_ONLY

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>

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

  struct Screen* pAppScreen;
  pAppScreen = OpenScreenTags(NULL,
                SA_LikeWorkbench, TRUE,
                SA_Title, "AmiDiff (C) 2018 by Uwe Rosner.",
                TAG_DONE);

  if (pAppScreen == NULL)
  {
    // Opening the screen failed
    closeLibs();
    return 30;
  }

  //
  // Calculating window size in depency of screen dimesions
  //

  int screenWidth = pAppScreen->Width;
  int screenHeight = pAppScreen->Height;
  int screenBarHeight = pAppScreen->BarHeight;

  int winWidth = screenWidth / 2;
  int winHeight = screenHeight - screenBarHeight - 1;

  //
  // Setting up the window properties
  //

  struct TagItem windowTags[] =
  {
    { WA_Left, 0 },
    { WA_Top, screenBarHeight + 1 },
    { WA_Width, winWidth },
    { WA_Height, winHeight },
    { WA_Title, (ULONG)"Left Diff Window" },
    { WA_Activate, TRUE },
    { WA_PubScreen, (ULONG)pAppScreen },
    { WA_IDCMP, IDCMP_MENUPICK },
    { WA_NewLookMenus, TRUE },  // Ignored before v39
    { TAG_DONE, NULL },
  };

  //
  // Opening the window
  //

  struct Window* pLeftWindow;
  pLeftWindow = OpenWindowTagList(NULL, windowTags);
  if(pLeftWindow == NULL)
  {
    // Opening the window failed
    CloseScreen(pAppScreen);
    closeLibs();
    return 40;
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
  pVisualInfo = (APTR*)GetVisualInfo(pAppScreen, TAG_END);
  if(pVisualInfo == NULL)
  {
    // Getting visual info has failed
    CloseWindow(pLeftWindow);
    CloseScreen(pAppScreen);
    closeLibs();
    return 40;

  }

  // Menu building step 1: Create the menu
  struct Menu* pAppMenu = CreateMenus(appMenuDefinition, TAG_END);
  if(pAppMenu == NULL)
  {
    // Creating the menu has failed
    FreeVisualInfo(pVisualInfo);
    CloseWindow(pLeftWindow);
    CloseScreen(pAppScreen);
    closeLibs();
    return 50;
  }

  // Menu building step 2: Layout the menu
  if(LayoutMenus(pAppMenu, pVisualInfo,
                 GTMN_NewLookMenus, TRUE, // Ignored before v39
                 TAG_END) == FALSE)
  {
    FreeMenus(pAppMenu);
    FreeVisualInfo(pVisualInfo);
    CloseWindow(pLeftWindow);
    CloseScreen(pAppScreen);
    closeLibs();
    return 60;
  }

  // Wire the menu strip into the window
  if(SetMenuStrip(pLeftWindow, pAppMenu) == FALSE)
  {
    FreeMenus(pAppMenu);
    FreeVisualInfo(pVisualInfo);
    CloseWindow(pLeftWindow);
    CloseScreen(pAppScreen);
    closeLibs();
    return 70;

  }

  handleWindowEvents(pLeftWindow, pAppMenu);

  ClearMenuStrip(pLeftWindow);
  FreeMenus(pAppMenu);
  FreeVisualInfo(pVisualInfo);
  CloseWindow(pLeftWindow);
  CloseScreen(pAppScreen);
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
