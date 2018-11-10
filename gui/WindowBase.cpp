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
    m_pMenu(NULL),
    m_pUserDataMenuItemToDisable(NULL)
{

}

WindowBase::~WindowBase()
{
  Close();
}

bool WindowBase::Open(APTR p_pUserDataMenuItemToDisable = NULL,
    InitialWindowPosition p_pInitialPosition = IWP_Center)
{
  m_pUserDataMenuItemToDisable = p_pUserDataMenuItemToDisable;
  //
  // Opening the window has to be done in ::Open of the derived classes.
  // Then these classes have to call this to auto-add menus etc.
  //

  if(!IsOpen())
  {
    // Opening failed
    return false;
  }

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

  if(p_pUserDataMenuItemToDisable != NULL)
  {
    m_pMenu->DisableMenuItem(m_pWindow, m_pUserDataMenuItemToDisable);
  }

  return true;
}

void WindowBase::Close()
{
  if(!IsOpen())
  {
    return;
  }

  if(m_pMenu != NULL && m_pUserDataMenuItemToDisable != NULL)
  {
    m_pMenu->EnableMenuItem(m_pWindow, m_pUserDataMenuItemToDisable);
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
