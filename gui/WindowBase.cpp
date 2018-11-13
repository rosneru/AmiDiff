#include <string.h>

#include <clib/alib_protos.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/asl.h>
#include <libraries/dos.h>
#include "WindowBase.h"

WindowBase::WindowBase(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort)
  : m_AppScreen(p_AppScreen),
    m_pMsgPort(p_pMsgPort),
    m_pWindow(NULL),
    m_pFirstGadget(NULL),
    m_WinWidth(160),
    m_WinHeight(120),
    m_pMenu(NULL),
    m_bInitialized(false),
    m_bIsFixed(false),
    m_InitialPosition(IP_Center),
    m_WindowFlags(0),
    m_WindowIdcmp(0),
    m_pMenuItemDisableAtOpen(NULL)
{

}

WindowBase::~WindowBase()
{
  Close();
}

bool WindowBase::Open(APTR p_pMenuItemDisableAtOpen)
{
  m_pMenuItemDisableAtOpen = p_pMenuItemDisableAtOpen;

  //
  // Initial validations
  //

  if(IsOpen())
  {
    // Not opening the window if it is already open
    // TODO Alternatively: bring window to front and return true;
    return false;
  }

  if(!m_bInitialized)
  {
    initialize();
  }

  //
  // Calculating window size etc in dependency of screen dimensions
  //
  int screenWidth = m_AppScreen.IntuiScreen()->Width;
  int screenHeight = m_AppScreen.IntuiScreen()->Height;
  int screenBarHeight = m_AppScreen.IntuiScreen()->BarHeight;

  int winLeft = 0;
  int winTop = 0;

  switch(m_InitialPosition)
  {
    case IP_Center:
      winTop = screenHeight / 2 - m_WinHeight / 2;
      winLeft = screenWidth / 2 - m_WinWidth / 2;
      break;

    case IP_Left:
      m_WinWidth = screenWidth / 2;
      m_WinHeight = screenHeight - screenBarHeight - 1;
      winTop = screenBarHeight + 1;
      winLeft = 0;
      break;

    case IP_Right:
      m_WinWidth = screenWidth / 2;
      m_WinHeight = screenHeight - screenBarHeight - 1;
      winTop = screenBarHeight + 1;
      winLeft = m_WinWidth;
      break;
  }

  if(!m_bIsFixed)
  {
    // Add a dragbar -> make window moveable
    m_WindowFlags |= WFLG_DRAGBAR;
  }

  //
  // Opening the window
  //
  m_pWindow = OpenWindowTags(NULL,
    WA_Left, winLeft,
    WA_Top, winTop,
    WA_Width, m_WinWidth,
    WA_Height, m_WinHeight,
    WA_Title, (ULONG) m_Title.C_str(),
    WA_Activate, TRUE,
    WA_PubScreen, (ULONG) m_AppScreen.IntuiScreen(),
    WA_Flags, m_WindowFlags,
    WA_SimpleRefresh, TRUE,
		WA_MinWidth, 120,
		WA_MinHeight, 90,
		WA_MaxWidth, -1,
		WA_MaxHeight, -1,
    WA_NewLookMenus, TRUE,          // Ignored before v39
    WA_Gadgets, m_pFirstGadget,
    TAG_END);

  if(!IsOpen())
  {
    // Opening failed
    return false;
  }

    // The window should be using this message port which might be shared
  // with other windows
  m_pWindow->UserPort = m_pMsgPort;

  ModifyIDCMP(m_pWindow, m_WindowIdcmp);

  if(m_pMenu == NULL)
  {
    // Opening successful, but at the moment there's no menu set to
    //the window
    return true;
  }

  // Set menu strip to the window
  if(m_pMenu->AttachToWindow(m_pWindow) == false)
  {
    return false;
  }

  if(p_pMenuItemDisableAtOpen != NULL)
  {
    m_pMenu->DisableMenuItem(m_pWindow, m_pMenuItemDisableAtOpen);
  }

  return true;
}

void WindowBase::Close()
{
  if(!IsOpen())
  {
    return;
  }

  if(m_pMenu != NULL && m_pMenuItemDisableAtOpen != NULL)
  {
    m_pMenu->EnableMenuItem(m_pWindow, m_pMenuItemDisableAtOpen);
  }

  if(m_pMenu != NULL)
  {
    m_pMenu->DetachFromWindow(m_pWindow);
  }

  closeWindowSafely();
}

bool WindowBase::IsOpen() const
{
  return m_pWindow != NULL;
}

const char* WindowBase::Title() const
{
  return m_Title.C_str();
}

void WindowBase::SetTitle(SimpleString p_NewTitle)
{
  m_Title = p_NewTitle;

  if(!IsOpen())
  {
    // Window is not open, so we don't change its title dynamically
    return;
  }

  // Call intuition function to set the window title
  // Note the ~0 inverts the value and is a value of -1
  SetWindowTitles(m_pWindow, m_Title.C_str(), (STRPTR) ~0);
}

void WindowBase::SetInitialPosition(InitialPosition p_InitialPosition)
{
  if(IsOpen())
  {
    return;
  }

  m_InitialPosition = p_InitialPosition;
}

void WindowBase::SetFixed(bool p_bFixWindow)
{
  if(IsOpen())
  {
    return;
  }

  m_bIsFixed = p_bFixWindow;
}

struct Window* WindowBase::IntuiWindow()
{
  return m_pWindow;
}

AppScreen& WindowBase::WindowScreen()
{
  return m_AppScreen;
}


void WindowBase::SetMenu(AppMenu* p_pMenu)
{
  if(p_pMenu == NULL)
  {
    // Given menu is NULL: do nothing
    return;
  }

  if(m_pMenu != NULL)
  {
    // There is already a menu set on this window : do nothing
    return;
  }

  m_pMenu = p_pMenu;


  if(!IsOpen())
  {
    // The window isn't open yet: don't attach the menu now
    return;
  }

  // The window is already open: attach the newly added menu to it
  m_pMenu->AttachToWindow(m_pWindow);
  return;
}


void WindowBase::setFlags(ULONG p_Flags)
{
  m_WindowFlags = p_Flags;
}


void WindowBase::setIDCMP(ULONG p_Idcmp)
{
  m_WindowIdcmp = p_Idcmp;
}

void WindowBase::setFirstGadget(struct Gadget* p_pFirstGadget)
{
  m_pFirstGadget = p_pFirstGadget;
}


struct Image* WindowBase::createImageObj(ULONG p_SysImageId, ULONG& p_Width, ULONG& p_Height)
{
  struct Image* pImage = (struct Image*) NewObject(
      NULL, SYSICLASS,
			SYSIA_Which, p_SysImageId,
			SYSIA_Size, SYSISIZE_MEDRES,
			SYSIA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
			TAG_END);

	if(pImage != NULL)
	{
	  GetAttr(IA_Width, pImage, &p_Width);
	  GetAttr(IA_Height, pImage, &p_Height);
	}

  return pImage;
}

void WindowBase::closeWindowSafely()
{
  if(!IsOpen())
  {
    return;
  }

  // We forbid here to keep out of race conditions with Intuition.
  Forbid();

  // Send back any messages for this window  that have not yet been
  // processed.
  stripIntuiMessages();

  // Clear UserPort so Intuition will not free it.
  m_pWindow->UserPort = NULL;

  // Tell intuition to stop sending more messages
  ModifyIDCMP(m_pWindow, 0L);

  // Turn multitasking back on
  Permit();

  CloseWindow(m_pWindow);
  m_pWindow = NULL;
}

void WindowBase::stripIntuiMessages()
{
  struct MsgPort* pMsgPort = m_pWindow->UserPort;
  struct Node* pSuccessor;
  struct IntuiMessage* pMessage;

  pMessage = (struct IntuiMessage*) pMsgPort->mp_MsgList.lh_Head;

  while(pSuccessor = pMessage->ExecMessage.mn_Node.ln_Succ)
  {
    if(pMessage->IDCMPWindow == m_pWindow)
    {
      // Intuition is about to free this message. Make sure that we
      // have politely sent it back.
      Remove((struct Node*) pMessage);
      ReplyMsg((struct Message*) pMessage);
    }

    pMessage = (struct IntuiMessage*) pSuccessor;
  }
}
