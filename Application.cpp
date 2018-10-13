#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>

#include "LinkedList.h"

#include "Command.h"
#include "CmdFileOpen.h"
#include "CmdQuit.h"

#include "Application.h"

Application::Application(int argc, char **argv)
  : m_Argc(argc),
    m_Argv(argv),
    m_pScreen(NULL),
    m_pLeftWin(NULL),
    m_pRightWin(NULL),
    m_pMenu(NULL),
    m_bExitRequested(false),
    m_pCmdOpenLeftFile(NULL),
    m_pCmdOpenRightFile(NULL),
    m_pCmdQuit(NULL)
{

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

  if(m_pCmdOpenLeftFile != NULL)
  {
    delete m_pCmdOpenLeftFile;
    m_pCmdOpenLeftFile = NULL;
  }

  if(m_pCmdOpenRightFile != NULL)
  {
    delete m_pCmdOpenRightFile;
    m_pCmdOpenRightFile = NULL;
  }

  if(m_pCmdQuit != NULL)
  {
    delete m_pCmdQuit;
    m_pCmdQuit = NULL;
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
  m_pScreen = new AppScreen("AmiDiff (C) 2018 by Uwe Rosner.");
  if (!m_pScreen->Open())
  {
    // Opening the screen failed
    Dispose();
    return false;
  }


  //
  // Opening the left window
  //
  m_pLeftWin = new DiffWindow(m_pScreen);
  if(!m_pLeftWin->Open(DiffWindow::LEFT))
  {
    // Opening the window failed
    Dispose();
    return false;
  }

  //
  // Opening the right window
  //
  m_pRightWin = new DiffWindow(m_pScreen);
  if(!m_pRightWin->Open(DiffWindow::RIGHT))
  {
    // Opening the window failed
    Dispose();
    return false;
  }

  //
  // Instantiating the commands
  //
  m_pCmdQuit = new CmdQuit("Quit the application", m_bExitRequested);
  m_pCmdOpenLeftFile = new CmdFileOpen("Open the left file", *m_pLeftWin);
  m_pCmdOpenRightFile = new CmdFileOpen("Open the right file", *m_pRightWin);


  // Fill the GadTools menu struct, supplying pointers to the commands
  // as nm_UserData
  struct NewMenu menuDefinition[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {  NM_ITEM,   "Open left file...",     "L", 0, 0, m_pCmdOpenLeftFile },
    {  NM_ITEM,   "Open right file...",    "R", 0, 0, m_pCmdOpenRightFile },
    {  NM_ITEM,   NM_BARLABEL,              0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",                  "Q", 0, 0, m_pCmdQuit },
    {  NM_END,    NULL,                     0 , 0, 0, 0 },
  };

  //
  // Creating the menu
  //
  m_pMenu = new AppMenu(m_pScreen->IntuiScreen());
  if(m_pMenu->Create(menuDefinition) == FALSE)
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

  //
  // If there are at least two command line arguments permitted,
  // (three for the if as the first one is the application name),
  // try to take the first two of them as file names and load them
  // into left and right window
  //
  if(m_Argc >= 3)
  {
    SimpleString fileNameLeft = m_Argv[1];
    SimpleString fileNameRight = m_Argv[2];

    m_pLeftWin->ReadFile(fileNameLeft);
    m_pRightWin->ReadFile(fileNameRight);
  }

  //
  // Wait-in-loop for menu actions etc
  //
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


  while (m_bExitRequested == false)
  {
    // Waiting for a signals from LeftWin or from RightWin
    Wait(1L << pWin1->UserPort->mp_SigBit |
         1L << pWin2->UserPort->mp_SigBit);

    struct IntuiMessage* pMsg;
    while ((m_bExitRequested == false) &&
          ((pMsg = (struct IntuiMessage *)GetMsg(pWin1->UserPort)) ||
           (pMsg = (struct IntuiMessage *)GetMsg(pWin2->UserPort))))
    {
      switch (pMsg->Class)
      {
        case IDCMP_MENUPICK:
        {
          UWORD menuNumber = pMsg->Code;
          struct MenuItem* pSelectedItem = ItemAddress(pMenu, menuNumber);

          if(pSelectedItem != NULL)
          {
            // Getting the user data from selected menu item
            APTR pUserData = GTMENUITEM_USERDATA(pSelectedItem);
            if(pUserData != NULL)
            {
              // Our menu user data always contains a pointer to a Command
              Command* pSelecedCommand = static_cast<Command*>(pUserData);

              // Execute this command
              pSelecedCommand->Execute();
            }
          }
          break;
        }

        case IDCMP_IDCMPUPDATE:
        {
          ULONG tagData = GetTagData(GA_ID, 0,
            (struct TagItem *)pMsg->IAddress);
          switch(tagData)
          {
            case GID_YPROP:
            {
              size_t newY = GetTagData(PGA_Top, 0,
                (struct TagItem *)pMsg->IAddress);

              if(pMsg->IDCMPWindow == m_pLeftWin->IntuiWindow())
              {
                m_pLeftWin->YChangedHandler(newY);
              }
              else if(pMsg->IDCMPWindow == m_pRightWin->IntuiWindow())
              {
                m_pRightWin->YChangedHandler(newY);
              }
              break;
            }
          }
          break;
        }

        case IDCMP_RAWKEY:
        {
          if(pMsg->Code == CURSORDOWN)
          {
            // Cursor *down* => scroll the text *up* in according window
            if(pMsg->IDCMPWindow == m_pLeftWin->IntuiWindow())
            {
              m_pLeftWin->YIncrease();
            }
            else if(pMsg->IDCMPWindow == m_pRightWin->IntuiWindow())
            {
              m_pRightWin->YIncrease();
            }
          }
          else if(pMsg->Code == CURSORUP)
          {
            // Cursor *up* => scroll the text *down* in according window
            if(pMsg->IDCMPWindow == m_pLeftWin->IntuiWindow())
            {
              m_pLeftWin->YDecrease();
            }
            else if(pMsg->IDCMPWindow == m_pRightWin->IntuiWindow())
            {
              m_pRightWin->YDecrease();
            }
          }
          break;
        }

        case IDCMP_NEWSIZE:
        {
          if(pMsg->IDCMPWindow == m_pLeftWin->IntuiWindow())
          {
            m_pLeftWin->Resized();
          }
          else if(pMsg->IDCMPWindow == m_pRightWin->IntuiWindow())
          {
            m_pRightWin->Resized();
          }

          break;
        }

        case IDCMP_REFRESHWINDOW:
        {
          if(pMsg->IDCMPWindow == m_pLeftWin->IntuiWindow())
          {
            m_pLeftWin->Refresh();
          }
          else if(pMsg->IDCMPWindow == m_pRightWin->IntuiWindow())
          {
            m_pRightWin->Refresh();
          }

          break;
        }

        case IDCMP_CLOSEWINDOW:
        {
          m_pCmdQuit->Execute();
          break;
        }

      }

      ReplyMsg((struct Message *)pMsg);
    }
  }
}
