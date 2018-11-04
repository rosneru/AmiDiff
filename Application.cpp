#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>

#include "LinkedList.h"

#include "Command.h"
#include "CmdPerformDiff.h"
#include "CmdOpenWindow.h"
#include "CmdQuit.h"

#include "TextDocument.h"

#include "Application.h"

Application::Application(int argc, char **argv)
  : m_pMsgPortAllWindows(NULL),
    m_Argc(argc),
    m_Argv(argv),
    m_pScreen(NULL),
    m_pOpenFilesWin(NULL),
    m_pLeftWin(NULL),
    m_pRightWin(NULL),
    m_pMenu(NULL),
    m_bExitRequested(false),
    m_pCmdOpenFilesWindow(NULL),
    m_pCmdDiff(NULL),
    m_pCmdQuit(NULL),
    m_pDiffFacade(NULL)
{
  // Create a message port for shared use with all windows
  m_pMsgPortAllWindows = CreateMsgPort();
}

Application::~Application()
{
  Dispose();
}

void Application::Dispose()
{
  if(m_pCmdOpenFilesWindow != NULL)
  {
    delete m_pCmdOpenFilesWindow;
    m_pCmdOpenFilesWindow = NULL;
  }

  if(m_pCmdDiff != NULL)
  {
    delete m_pCmdDiff;
    m_pCmdDiff = NULL;
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

  if(m_pOpenFilesWin != NULL)
  {
    delete m_pOpenFilesWin;
    m_pOpenFilesWin = NULL;
  }

  if(m_pMenu != NULL)
  {
    delete m_pMenu;
    m_pMenu = NULL;
  }

  if(m_pScreen != NULL)
  {
    delete m_pScreen;
    m_pScreen = NULL;
  }

  if(m_pDiffFacade != NULL)
  {
    delete m_pDiffFacade;
    m_pDiffFacade = NULL;
  }


  if(m_pMsgPortAllWindows != NULL)
  {
    DeleteMsgPort(m_pMsgPortAllWindows);
    m_pMsgPortAllWindows = NULL;
  }
}

bool Application::Run()
{
  // If the message port has not been created, the app can't start.
  if(m_pMsgPortAllWindows == NULL)
  {
    return false;
  }

  // Create DiffFacade
  m_pDiffFacade = new AmigaDiffFacade();

  //
  // If there are at least two command line arguments permitted, take
  // the first two of them (argv[1] and argv[2]  as file names and load
  // them into left and right window.
  //
  if(m_Argc >= 3)
  {
    m_pDiffFacade->SetLeftFilePath(m_Argv[1]);
    m_pDiffFacade->SetRightFilePath(m_Argv[2]);
  }


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
  // Creating left and right diff windows but not opening them yet
  //
  m_pLeftWin = new TextWindow(m_pScreen, m_pMsgPortAllWindows);
  m_pRightWin = new TextWindow(m_pScreen, m_pMsgPortAllWindows);

  //
  // Instantiating the commands
  //
  m_pCmdQuit = new CmdQuit(m_bExitRequested);
  m_pCmdDiff = new CmdPerformDiff(*m_pDiffFacade);

  //
  // Now that the CmdDiff is available the OpenFilesWindow can be
  // created.
  //
  m_pOpenFilesWin = new OpenFilesWindow(m_pScreen, m_pMsgPortAllWindows,
    *m_pDiffFacade);

  m_pCmdOpenFilesWindow = new CmdOpenWindow(*m_pOpenFilesWin);

  //
  // Fill the GadTools menu struct, supplying pointers to the commands
  // as nm_UserData. With this behavior there is no complicated
  // evalution needed to detect which menu item was clicked. Just
  // the ->Execute of the (by then anonymous) command has to be called.
  //
  struct NewMenu menuDefinition[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {  NM_ITEM,   "Open files...",         "O", 0, 0, m_pCmdOpenFilesWindow },
//    {  NM_ITEM,   "Time statistics...",    "T", 0, 0, m_pCmdStatistics }, // TODO
    {  NM_ITEM,   NM_BARLABEL,              0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",                  "Q", 0, 0, m_pCmdQuit },
    {  NM_END,    NULL,                     0 , 0, 0, 0 },
  };

  //
  // Creating the menu
  //
  m_pMenu = new AppMenu(m_pScreen);
  if(m_pMenu->Create(menuDefinition) == false)
  {
    Dispose();
    return false;
  }

  //
  // Installing menu to all windows
  //
  m_pLeftWin->SetMenu(m_pMenu);
  m_pRightWin->SetMenu(m_pMenu);
  m_pOpenFilesWin->SetMenu(m_pMenu);

  m_pOpenFilesWin->Open(m_pCmdOpenFilesWindow);

  //
  // Wait-in-loop for menu actions etc
  //
  intuiEventLoop();

  return true;

}

void Application::intuiEventLoop()
{
  struct IntuiMessage* pMsg;
  do
  {
    Wait(1L << m_pMsgPortAllWindows->mp_SigBit);
    while (pMsg = (struct IntuiMessage *) GT_GetIMsg(m_pMsgPortAllWindows))
    {
      // Get all data we need from message
      ULONG msgClass = pMsg->Class;
      UWORD msgCode = pMsg->Code;
      APTR msgIAddress = pMsg->IAddress;
      struct Window* msgWindow = pMsg->IDCMPWindow;

      // When we're through with a message, reply
      GT_ReplyIMsg(pMsg);

      if(msgClass == IDCMP_MENUPICK)
      {
        //
        // Menupick messages are handled here
        //
        UWORD menuNumber = msgCode;
        struct MenuItem* pSelectedItem = ItemAddress(m_pMenu->IntuiMenu(), menuNumber);

        if(pSelectedItem != NULL)
        {
          // Getting the user data from selected menu item
          APTR pUserData = GTMENUITEM_USERDATA(pSelectedItem);
          if(pUserData != NULL)
          {
            // Our menu user data always contain a pointer to a Command
            Command* pSelecedCommand = static_cast<Command*>(pUserData);

            // Execute this command
            pSelecedCommand->Execute();
          }
        }
      }
      else
      {
        //
        // All other messages are handled in the appropriate window
        //
        if(m_pLeftWin->IsOpen() && msgWindow == m_pLeftWin->IntuiWindow())
        {
          m_pLeftWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
        else if(m_pRightWin->IsOpen() && msgWindow == m_pRightWin->IntuiWindow())
        {
          m_pRightWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
        else if(m_pOpenFilesWin->IsOpen() && msgWindow == m_pOpenFilesWin->IntuiWindow())
        {
          m_pOpenFilesWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
      }

      if(!m_pLeftWin->IsOpen() && !m_pRightWin->IsOpen() &&
         !m_pOpenFilesWin->IsOpen())
      {
        // All windows are close: exit
        m_bExitRequested = true;
        break;
      }
    }
  }
  while(!m_bExitRequested);
}
