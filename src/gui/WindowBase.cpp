#include <string.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/wb_protos.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <intuition/imageclass.h>

#include "WindowBase.h"

extern struct GfxBase* GfxBase;

WindowBase::WindowBase(ScreenBase& screen,
                       struct MsgPort* pIdcmpMsgPort,
                       AMenu* pMenu)
  : m_Screen(screen),
    m_pIdcmpMsgPort(pIdcmpMsgPort),
    m_pWindow(NULL),
    m_bBorderless(false),
    m_Left(0),
    m_Top(0),
    m_Width(0),
    m_Height(0),
    m_pMenu(pMenu),
    m_pTextFont(((struct GfxBase *)GfxBase)->DefaultFont),
    m_bIsFixed(false),
    m_InitialPosition(IP_Center),
    m_WindowFlags(0),
    m_WindowIdcmp(0),
    m_pFirstGadget(NULL),
    m_pAppWindowPort(NULL),
    m_pAppWindow(NULL),
    m_AppWindowId(0)
{
}


WindowBase::~WindowBase()
{
  Close();
}

void WindowBase::Resized()
{
  // Resizable childs should override this and re-paint their contents
  // according the new size.
}

bool WindowBase::Open(InitialPosition initialPos)
{
  m_InitialPosition = initialPos;

  //
  // Initial validations
  //

  if(IsOpen())
  {
    // Not opening the window if it is already open
    // TODO Alternatively: bring window to front and return true;
    return true;
  }

  //
  // Calculating window size etc in dependency of screen dimensions
  //
  int screenWidth = m_Screen.IntuiScreen()->Width;
  int screenHeight = m_Screen.IntuiScreen()->Height;
  int screenBarHeight = m_Screen.IntuiScreen()->BarHeight;

  switch(m_InitialPosition)
  {
    case IP_Center:
      if(m_Width == 0)
      {
        // Initial window width not set in initialize()
        m_Width = screenWidth * 60 / 10;
      }

      if(m_Height == 0)
      {
        // Initial window heigth not set in initialize()
        m_Height = screenHeight * 60 / 10;
      }

      m_Top = screenHeight / 2 - m_Height / 2;
      m_Left = screenWidth / 2 - m_Width / 2;
      break;

    case IP_Fill:
      m_Width = screenWidth;
      m_Height = screenHeight - screenBarHeight - 1;
      m_Top = screenBarHeight + 1;
      m_Left = 0;
      break;

    case IP_Left:
      m_Width = screenWidth / 2;
      m_Height = screenHeight - screenBarHeight - 1;
      m_Top = screenBarHeight + 1;
      m_Left = 0;
      break;

    case IP_Right:
      m_Width = screenWidth / 2;
      m_Height = screenHeight - screenBarHeight - 1;
      m_Top = screenBarHeight + 1;
      m_Left = m_Width;
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

  // Open the window
  m_pWindow = OpenWindowTags(NULL,
                             WA_SmartRefresh, TRUE,
                             WA_Left, m_Left,
                             WA_Top, m_Top,
                             WA_Width, m_Width,
                             WA_Height, m_Height,
                             WA_Title, (ULONG) m_Title.c_str(),
                             WA_Activate, TRUE,
                             WA_PubScreen, (UBYTE*) m_Screen.IntuiScreen(),
                             WA_Flags, m_WindowFlags,
                             WA_MinWidth, 230,
                             WA_MinHeight, 64,
                             WA_MaxWidth, ~0,
                             WA_MaxHeight, ~0,
                             WA_NewLookMenus, TRUE,
                             WA_Borderless, m_bBorderless,
                             WA_Gadgets, m_pFirstGadget,
                             TAG_DONE);

  if(!IsOpen())
  {
    // Opening failed
    return false;
  }

  m_Screen.IncreaseNumOpenWindows();

  // The window uses this message port which can be the same as used by
  // other windows
  m_pWindow->UserPort = m_pIdcmpMsgPort;

  ModifyIDCMP(m_pWindow, m_WindowIdcmp);

  // Create an AppWindow if requested
  if(m_pAppWindowPort != NULL)
  {
    m_pAppWindow = AddAppWindow(m_AppWindowId,
                                0,
                                m_pWindow,
                                m_pAppWindowPort,
                                TAG_DONE, NULL);
  }

  if(m_pMenu == NULL)
  {
    // Opening successful, but at the moment there's no menu set to
    //the window
    return true;
  }

  // Create the menu if; returns true if already done
  if(m_pMenu->Create(m_Screen) == true)
  {
    // Attach the menu to the window
    m_pMenu->AttachToWindow(m_pWindow);
  }

  return true;
}


void WindowBase::Close()
{
  if(!IsOpen())
  {
    return;
  }

  if(m_pMenu != NULL)
  {
    m_pMenu->DetachFromWindow(m_pWindow);
  }

  if(m_pAppWindow != NULL)
  {
    RemoveAppWindow(m_pAppWindow);
    m_pAppWindow = NULL;
  }

  closeWindowSafely();
  m_Screen.DecreaseNumOpenWindows();
}


bool WindowBase::IsOpen() const
{
  return m_pWindow != NULL;
}


const char* WindowBase::Title() const
{
  return m_Title.c_str();
}


void WindowBase::SetTitle(std::string newTitle)
{
  m_Title = newTitle;

  if(!IsOpen())
  {
    // Window is not open, so we don't change its title dynamically
    return;
  }

  // Call intuition function to set the window title
  // Note the ~0 inverts the value and is a value of -1
  SetWindowTitles(m_pWindow, m_Title.c_str(), (STRPTR) ~0);
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
  m_Left = left;
  m_Top = top;
  m_Width = width;
  m_Height = height;
}


void WindowBase::SetFixed(bool bFixWindow)
{
  if(IsOpen())
  {
    return;
  }

  m_bIsFixed = bFixWindow;
}


void WindowBase::SetBorderless(bool bBorderless)
{
  if(IsOpen())
  {
    return;
  }

  m_bBorderless = bBorderless;
}


void WindowBase::EnableAppWindow(struct MsgPort* pAppWindowPort,
                                 ULONG appWindowId)
{
  if(IsOpen())
  {
    return;
  }

  m_pAppWindowPort = pAppWindowPort;
  m_AppWindowId = appWindowId;
}


struct Window* WindowBase::IntuiWindow()
{
  return m_pWindow;
}


void WindowBase::SetMenu(AMenu* pMenu)
{
  if(pMenu == NULL)
  {
    // Given menu is NULL: do nothing
    return;
  }

  if(m_pMenu != NULL)
  {
    // There is already a menu set on this window : do nothing
    return;
  }

  m_pMenu = pMenu;


  if(!IsOpen())
  {
    // The window isn't open yet: don't attach the menu now
    return;
  }

  // Create the menu; returns true if already done
  if(m_pMenu->Create(m_Screen) == true)
  {
    // Attach the menu to the window
    m_pMenu->AttachToWindow(m_pWindow);
  }

  return;
}

AMenu* WindowBase::Menu()
{
  return m_pMenu;
}


void WindowBase::addFlags(ULONG flags)
{
  m_WindowFlags |= flags;
}


void WindowBase::addIDCMP(ULONG idcmp)
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


struct Image* WindowBase::createImageObj(ULONG sysImageId,
                                         ULONG& width,
                                         ULONG& height)
{
  struct Image* pImage = (struct Image*) NewObject(
      NULL, SYSICLASS,
      SYSIA_Which, sysImageId,
      SYSIA_Size, SYSISIZE_MEDRES,
      SYSIA_DrawInfo, m_Screen.IntuiDrawInfo(),
      TAG_DONE);

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
  struct MsgPort* pIdcmpMsgPort = m_pWindow->UserPort;
  struct Node* pSuccessor;
  struct IntuiMessage* pMessage;

  pMessage = (struct IntuiMessage*) pIdcmpMsgPort->mp_MsgList.lh_Head;

  while((pSuccessor = pMessage->ExecMessage.mn_Node.ln_Succ) != NULL)
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
