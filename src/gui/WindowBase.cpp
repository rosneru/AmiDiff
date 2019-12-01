#include <string.h>

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


WindowBase::WindowBase(AppScreen& appScreen,
                       struct MsgPort* pMsgPort,
                       int& numOpenWindows)
  : m_AppScreen(appScreen),
    m_pMsgPort(pMsgPort),
    m_NumOpenWindows(numOpenWindows),
    m_pWindow(NULL),
    m_bBorderless(false),
    m_bSmartRefresh(false),
    m_WinLeft(0),
    m_WinTop(0),
    m_WinWidth(0),
    m_WinHeight(0),
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


bool WindowBase::Open(const APTR pMenuItemDisableAtOpen,
                      InitialPosition initialPosition)
{
  m_pMenuItemDisableAtOpen = pMenuItemDisableAtOpen;
  m_InitialPosition = initialPosition;

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
      if(m_WinWidth == 0)
      {
        // Initial window width not set in initialize()
        m_WinWidth = screenWidth * 60 / 10;
      }

      if(m_WinHeight == 0)
      {
        // Initial window heigth not set in initialize()
        m_WinHeight = screenHeight * 60 / 10;
      }

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

    case IP_Explicit:
      // Nothing to do
      break;
  }

  if(!m_bIsFixed)
  {
    // Add a dragbar -> make window movable
    m_WindowFlags |= WFLG_DRAGBAR;
  }

  ULONG smartOrSimpleRefresh = WA_SmartRefresh;
  if(m_bSmartRefresh == false)
  {
    // Simple-refresh mode
    smartOrSimpleRefresh = WA_SimpleRefresh;
  }

  //
  // Open the window
  //
  m_pWindow = OpenWindowTags(NULL,
    smartOrSimpleRefresh, TRUE,
    WA_Left, m_WinLeft,
    WA_Top, m_WinTop,
    WA_Width, m_WinWidth,
    WA_Height, m_WinHeight,
    WA_Title, (ULONG) m_Title.C_str(),
    WA_Activate, TRUE,
    WA_PubScreen, (UBYTE*) m_AppScreen.IntuiScreen(),
//    WA_CustomScreen, (UBYTE*) m_AppScreen.IntuiScreen(),
    WA_Flags, m_WindowFlags,
    WA_MinWidth, 120,
    WA_MinHeight, 90,
    WA_MaxWidth, -1,
    WA_MaxHeight, -1,
    WA_NewLookMenus, TRUE,          // Ignored before v39
    WA_Borderless, m_bBorderless,
    WA_Gadgets, m_pFirstGadget,
    TAG_DONE);

  if(!IsOpen())
  {
    // Opening failed
    return false;
  }

  // The window uses this message port which can be the same as used by
  // other windows
  m_pWindow->UserPort = m_pMsgPort;

  ModifyIDCMP(m_pWindow, m_WindowIdcmp);

  m_NumOpenWindows++;

  if(m_pMenu == NULL)
  {
    // Opening successful, but at the moment there's no menu set to
    //the window
    return true;
  }

  // Set menu strip to the window
  if(m_pMenu->AttachToWindow(m_pWindow) == false)
  {
    // TODO React somehow on this failing?
    return true;
  }

  if(pMenuItemDisableAtOpen != NULL)
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
  m_NumOpenWindows--;
}


bool WindowBase::IsOpen() const
{
  return m_pWindow != NULL;
}


const APTR WindowBase::DisabledMenuItem() const
{
  return m_pMenuItemDisableAtOpen;
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


void WindowBase::SetInitialDimension(ULONG left,
                                     ULONG top,
                                     ULONG width,
                                     ULONG height)
{
  if(IsOpen())
  {
    return;
  }

  m_InitialPosition = WindowBase::IP_Explicit;
  m_WinLeft = left;
  m_WinTop = top;
  m_WinWidth = width;
  m_WinHeight = height;
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
  long width, long height)
{
  if(!IsOpen())
  {
    return;
  }

  ChangeWindowBox(p_Left, p_Top, width, height);
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


void WindowBase::setFlags(ULONG flags)
{
  m_WindowFlags |= flags;
}


void WindowBase::setIDCMP(ULONG idcmp)
{
  m_WindowIdcmp |= idcmp;
}


void WindowBase::setFirstGadget(struct Gadget* pFirstGadget)
{
  if(m_pFirstGadget != NULL)
  {
    return;
  }

  m_pFirstGadget = pFirstGadget;
}


struct Gadget* WindowBase::getFirstGadget()
{
  return m_pFirstGadget;
}


struct Gadget* WindowBase::getLastGadget()
{
  if(m_pFirstGadget == NULL)
  {
    return NULL;
  }

  struct Gadget* pGadget = m_pFirstGadget->NextGadget;
  while(pGadget != NULL)
  {
    if(pGadget->NextGadget == NULL)
    {
      break;
    }

    pGadget = pGadget->NextGadget;
  }

  return pGadget;

}


struct Image* WindowBase::createImageObj(ULONG sysImageId, ULONG& width, ULONG& height)
{
  struct Image* pImage = (struct Image*) NewObject(
      NULL, SYSICLASS,
      SYSIA_Which, sysImageId,
      SYSIA_Size, SYSISIZE_MEDRES,
      SYSIA_DrawInfo, m_AppScreen.IntuiDrawInfo(),
      TAG_END);

  if(pImage != NULL)
  {
    GetAttr(IA_Width, pImage, &width);
    GetAttr(IA_Height, pImage, &height);
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

