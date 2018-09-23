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

  int leftWinSig = 1L << m_pLeftWin->IntuiWindow()->UserPort->mp_SigBit;
  int rightWinSig = 1L << m_pRightWin->IntuiWindow()->UserPort->mp_SigBit;

  bool bExit = false;
  do
  {
    int signals = Wait(leftWinSig | rightWinSig);
    struct IntuiMessage* pMsg = NULL;
    do
    {
      if(signals & leftWinSig)
      {
        pMsg = (struct IntuiMessage*) GetMsg(m_pLeftWin->IntuiWindow()->UserPort);
      }
      else if(signals & rightWinSig)
      {
        pMsg = (struct IntuiMessage*) GetMsg(m_pRightWin->IntuiWindow()->UserPort);
      }

      switch (pMsg->Class)
      {
      case IDCMP_MENUPICK:
        UWORD menuNumber = pMsg->Code;
        struct MenuItem* pItem = NULL;

        while ((menuNumber != MENUNULL) && (bExit == false))
        {
          pItem = ItemAddress(m_pMenu->IntuiMenu(), menuNumber);

          // Which item in menu and submenu was selected?
          UWORD menuNum = MENUNUM(menuNumber);
          UWORD itemNum = ITEMNUM(menuNumber);
          UWORD subNum  = SUBNUM(menuNumber);

          /* stop if quit is selected. */
          if ((menuNum == 0) && (itemNum == 3))
          {
            bExit = true;
          }

          menuNumber = pItem->NextSelect;
        }
        break;
      }

      ReplyMsg((struct Message *)pMsg);


    }
    while((bExit == false) && (pMsg != NULL));

  }
  while(bExit == false);
}
