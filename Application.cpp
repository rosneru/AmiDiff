#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include "Application.h"

Application::Application()
{
  m_pScreen = NULL;
  m_pLeftWin = NULL;
  m_pRightWin = NULL;
  m_pMenu = NULL;
}

Application::~Application()
{
  Dispose();
}

void Application::Dispose()
{
  if(m_pMenu != NULL)
  {
    delete m_pMenu;
    m_pMenu = NULL;
  }

  if(m_pRightWin != NULL)
  {
    delete m_pRightWin;
    m_pRightWin = NULL;
  }

  if(m_pLeftWin != NULL)
  {
    delete m_pLeftWin;
    m_pLeftWin = NULL;
  }

  if(m_pScreen != NULL)
  {
    delete m_pScreen;
    m_pScreen = NULL;
  }

}

bool Application::Run()
{
  //
  // Opening the screen
  //

  m_pScreen = new AppScreen();
  if (!m_pScreen->Open())
  {
    // Opening the screen failed
    Dispose();
    return false;
  }


  //
  // Opening the left window
  //
  m_pLeftWin = new DiffWindow(m_pScreen->IntuiScreen());
  if(!m_pLeftWin->Open(DiffWindow::LEFT))
  {
    // Opening the window failed
    Dispose();
    return false;
  }

  //
  // Opening the right window
  //
  m_pRightWin = new DiffWindow(m_pScreen->IntuiScreen());
  if(!m_pRightWin->Open(DiffWindow::RIGHT))
  {
    // Opening the window failed
    Dispose();
    return false;
  }

  //
  // Creating the menu
  //

  m_pMenu = new AppMenu(m_pScreen->IntuiScreen());
  if(m_pMenu->Create() == FALSE)
  {
    Dispose();
    return false;
  }

  //
  // Installing menu to left window
  //

  if(m_pMenu->BindToWindow(m_pLeftWin->IntuiWindow()) == FALSE)
  {
    Dispose();
    return false;
  }

  //
  // Installing menu to left window
  //

  if(m_pMenu->BindToWindow(m_pRightWin->IntuiWindow()) == FALSE)
  {
    Dispose();
    return false;
  }

  intuiEventLoop();

  Dispose();
  return true;

}

void Application::intuiEventLoop()
{
  //
  // Waiting for messages from Intuition
  //

  struct Window* pWin1 = m_pLeftWin->IntuiWindow();
  struct Window* pWin2 = m_pRightWin->IntuiWindow();
  struct Menu* pMenu = m_pMenu->IntuiMenu();

  bool bExit = false;
  while (bExit == false)
  {
    // Waiting for a signals from LeftWin or from RightWin
    Wait(1L << pWin1->UserPort->mp_SigBit |
         1L << pWin2->UserPort->mp_SigBit);

    struct IntuiMessage* pMsg;
    while ((bExit == false) &&
           ((pMsg = (struct IntuiMessage *)GetMsg(pWin1->UserPort)) ||
           (pMsg = (struct IntuiMessage *)GetMsg(pWin2->UserPort))))
    {
      switch (pMsg->Class)
      {
      case IDCMP_MENUPICK:
          UWORD menuNumber = pMsg->Code;
          while ((menuNumber != MENUNULL) && (bExit == false))
          {
            struct MenuItem* pSelectedItem = ItemAddress(pMenu, menuNumber);

            // process the item here!
            UWORD menuNum = MENUNUM(menuNumber);
            UWORD itemNum = ITEMNUM(menuNumber);
            UWORD subNum  = SUBNUM(menuNumber);

            // stop if quit is selected.
            if ((menuNum == 0) && (itemNum == 3))
            {
              bExit = true;
            }

            menuNumber = pSelectedItem->NextSelect;
          }

          break;
      }

      ReplyMsg((struct Message *)pMsg);
    }
  }
}
