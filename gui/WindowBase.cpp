#include <string.h>

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
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
    m_bBorderless(false),
    m_bSmartRefresh(false),
    m_WinLeft(0),
    m_WinTop(0),
    m_WinWidth(160),
    m_WinHeight(120),
    m_pFirstGadget(NULL),
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

  switch(m_InitialPosition)
  {
    case IP_Center:
      m_WinTop = screenHeight / 2 - m_WinHeight / 2;
      m_WinLeft = screenWidth / 2 - m_WinWidth / 2;
      break;

    case IP_Fill:
      m_WinWidth = screenWidth;
      m_WinHeight = screenHeight - screenBarHeight - 1;
      m_WinTop = screenBarHeight + 1;
      m_WinLeft = 0;
      break;

    case IP_Left:
      m_WinWidth = screenWidth / 2;
      m_WinHeight = screenHeight - screenBarHeight - 1;
      m_WinTop = screenBarHeight + 1;
      m_WinLeft = 0;
      break;

    case IP_Right:
      m_WinWidth = screenWidth / 2;
      m_WinHeight = screenHeight - screenBarHeight - 1;
      m_WinTop = screenBarHeight + 1;
      m_WinLeft = m_WinWidth;
      break;
  }

  if(!m_bIsFixed)
  {
    // Add a dragbar -> make window movable
    m_WindowFlags |= WFLG_DRAGBAR;
  }

  //
  // Opening the window
  //
  // NOTE: Two separate calls because WA_SmartRefresh and
  //       WA_SimpleRefresh mutually excluding each other and should
  //       only be permitted if true.
  //
  if(m_bSmartRefresh == true)
  {
    m_pWindow = OpenWindowTags(NULL,
      WA_Left, m_WinLeft,
      WA_Top, m_WinTop,
      WA_Width, m_WinWidth,
      WA_Height, m_WinHeight,
      WA_Title, (ULONG) m_Title.C_str(),
      WA_Activate, TRUE,
      WA_PubScreen, (ULONG) m_AppScreen.IntuiScreen(),
      WA_Flags, m_WindowFlags,
      WA_MinWidth, 120,
      WA_MinHeight, 90,
      WA_MaxWidth, -1,
      WA_MaxHeight, -1,
      WA_NewLookMenus, TRUE,          // Ignored before v39
      WA_Borderless, m_bBorderless,
      WA_Gadgets, m_pFirstGadget,
      WA_SmartRefresh, TRUE,
      TAG_END);
  }
  else
  {
    m_pWindow = OpenWindowTags(NULL,
      WA_Left, m_WinLeft,
      WA_Top, m_WinTop,
      WA_Width, m_WinWidth,
      WA_Height, m_WinHeight,
      WA_Title, (ULONG) m_Title.C_str(),
      WA_Activate, TRUE,
      WA_PubScreen, (ULONG) m_AppScreen.IntuiScreen(),
      WA_Flags, m_WindowFlags,
      WA_MinWidth, 120,
      WA_MinHeight, 90,
      WA_MaxWidth, -1,
      WA_MaxHeight, -1,
      WA_NewLookMenus, TRUE,          // Ignored before v39
      WA_Borderless, m_bBorderless,
      WA_Gadgets, m_pFirstGadget,
      WA_SimpleRefresh, TRUE,
      TAG_END);
  }

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

void WindowBase::SetInitialPosition(InitialPosition p_InitialPosition,
  long p_Left, long p_Top, long p_Width, long p_Height)
{
  if(IsOpen())
  {
    return;
  }

  m_InitialPosition = p_InitialPosition;

  if(p_InitialPosition == WindowBase::IP_Explicit)
  {
    m_WinLeft = p_Left;
    m_WinTop = p_Top;
    m_WinWidth = p_Width;
    m_WinHeight = p_Height;
  }
}

void WindowBase::SetFixed(bool p_bFixWindow)
{
  if(IsOpen())
  {
    return;
  }

  m_bIsFixed = p_bFixWindow;
}


void WindowBase::SetBorderless(bool p_bBorderless)
{
  if(IsOpen())
  {
    return;
  }

  m_bBorderless = p_bBorderless;
}

void WindowBase::SetSmartRefresh(bool p_bSmartRefresh)
{
  if(IsOpen())
  {
    return;
  }

  m_bSmartRefresh = p_bSmartRefresh;
}

void WindowBase::Move(long p_DX, long p_DY)
{
  if(!IsOpen())
  {
    return;
  }

  MoveWindow(m_pWindow, p_DX, p_DY);
  m_WinLeft = m_pWindow->LeftEdge;
  m_WinTop = m_pWindow->TopEdge;
}

void WindowBase::Size(long p_DX, long p_DY)
{
  if(!IsOpen())
  {
    return;
  }

  SizeWindow(m_pWindow, p_DX, p_DY);
  m_WinWidth = m_pWindow->Width;
  m_WinHeight = m_pWindow->Height;
}

void WindowBase::ChangeWindowBox(long p_Left, long p_Top,
  long p_Width, long p_Height)
{
  if(!IsOpen())
  {
    return;
  }

  ChangeWindowBox(p_Left, p_Top, p_Width, p_Height);
  m_WinLeft = m_pWindow->LeftEdge;
  m_WinTop = m_pWindow->TopEdge;
  m_WinWidth = m_pWindow->Width;
  m_WinHeight = m_pWindow->Height;
}

struct Window* WindowBase::IntuiWindow()
{
  return m_pWindow;
}

AppScreen& WindowBase::WindowScreen()
{
  return m_AppScreen;
}


void WindowBase::SetMenu(ApplicationMenu* p_pMenu)
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
  m_WindowFlags |= p_Flags;
}


void WindowBase::setIDCMP(ULONG p_Idcmp)
{
  m_WindowIdcmp |= p_Idcmp;
}

void WindowBase::setFirstGadget(struct Gadget* p_pFirstGadget)
{
  if(m_pFirstGadget == NULL)
  {
    m_pFirstGadget = p_pFirstGadget;
  }
  else
  {
    struct Gadget* pGadget = m_pFirstGadget->NextGadget;
    while(pGadget != NULL)
    {
      if(pGadget->NextGadget == NULL)
      {
        pGadget->NextGadget = p_pFirstGadget;
        break;
      }
      else
      {
        pGadget = pGadget->NextGadget;
      }
    }
  }
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
