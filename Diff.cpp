// Prototypes
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/dos_protos.h>
#include <clib/window_protos.h>
#include <clib/layout_protos.h>
#include <clib/button_protos.h>

// Types and constants
#include <classes/window.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>

// ReAction-specific includes
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

struct Library* IntuitionBase;
struct Library* DosBase;
struct Library* WindowBase;
struct Library* LayoutBase;
struct Library* ButtonBase;

// Forward declarations
void closeLibs();

int main()
{
  Object* pWindow;
  Object* pLayout;
  Object* pButton;

  struct Window* pIntuiWin;

  IntuitionBase = OpenLibrary("intuition.library", 39);
  DosBase       = OpenLibrary("dos.library", 39);
  WindowBase    = OpenLibrary("window.class", 44);
  LayoutBase    = OpenLibrary("gadgets/layout.gadget", 44);
  ButtonBase    = OpenLibrary("gadgets/button.gadget", 44);

  if((!IntuitionBase)
    || (!WindowBase)
    || (!LayoutBase)
    || (!ButtonBase))
  {
    closeLibs();
    exit(20);
  }

  // Declare a message port for the window
  struct MsgPort* pAppPort;

  // Define a default ReAction window
  pWindow = (Object*)WindowObject,
    WA_ScreenTitle, "DiffViewer (C) 2018 by Uwe Rosner.",
    WA_Title, "DiffViewer",
    WA_Activate, TRUE,
    WA_DepthGadget, TRUE,
    WA_DragBar, TRUE,
    WA_CloseGadget, TRUE,
    WA_SizeGadget, TRUE,
    WA_Width, 640,
    WA_Height, 256,
    WINDOW_IconifyGadget, TRUE,
    WINDOW_IconTitle, "DiffViewer",
    WINDOW_AppPort, pAppPort,
    WINDOW_Position, WPOS_CENTERMOUSE,
    WINDOW_ParentGroup, VLayoutObject,
      LAYOUT_SpaceOuter, TRUE,
      LAYOUT_DeferLayout, TRUE,
    EndGroup,
  EndWindow;

  // Open the window
  pIntuiWin = RA_OpenWindow(pWindow);
  if(!pIntuiWin)
  {
    closeLibs();
    exit(30);
  }

  // Get the bit mask of the created window from the User port
  ULONG winBitMask = (1L << pAppPort->mp_SigBit);

  // Get the signal mask of the created window
  ULONG winSignal;
  GetAttr(WINDOW_SigMask, pWindow, &winSignal);

  bool bDone = false;
  while(!bDone)
  {
    ULONG winWait = (winSignal
  }

/*
      while (!done)
      {
        wait = Wait( signal | SIGBREAKF_CTRL_C | app);
        if (wait & SIGBREAKF_CTRL_C)
        {
          done = TRUE;
        } else {
        while (( result = RA_HandleInput(objects[OID_MAIN], &code)) != WMHI_LASTMSG)
        {
          switch (result & WMHI_CLASSMASK)
          {
            case WMHI_CLOSEWINDOW:
              windows[WID_MAIN]=NULL;
              done=TRUE;
              break;
          }
        } /* End while */
      } /* End if */
    } /* End while */
*/


  closeLibs();
  exit(0);
}


void closeLibs()
{
  CloseLibrary(ButtonBase);
  CloseLibrary(WindowBase);
  CloseLibrary(LayoutBase);
  CloseLibrary(DosBase);
  CloseLibrary(IntuitionBase);

}