#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

#include "Command.h"
#include "CmdPerformDiff.h"
#include "CmdOpenWindow.h"
#include "CmdQuit.h"

#include "TextDocument.h"

#include "Application.h"

Application::Application(struct MsgPort* p_pMsgPortAllWindows,
  const SimpleString& p_PubScreenName)
  : m_pMsgPortAllWindows(p_pMsgPortAllWindows),
    m_PubScreenName(p_PubScreenName),
    m_bExitRequested(false),
    m_Screen(),
    m_DiffWindow(m_Screen, m_pMsgPortAllWindows),
    m_DiffFacade(m_DiffWindow),
    m_OpenFilesWin(m_Screen, m_pMsgPortAllWindows, m_DiffFacade),
    m_CmdDiff(m_DiffFacade),
    m_CmdQuit(m_bExitRequested),
    m_CmdOpenFilesWindow(m_OpenFilesWin),
    m_Menu(m_Screen)
{
}

Application::~Application()
{

}

void Application::SetLeftFilePath(const SimpleString& p_LeftFilePath)
{
  m_DiffFacade.SetLeftFilePath(p_LeftFilePath);
}

void Application::SetRightFilePath(const SimpleString& p_RightFilePath)
{
  m_DiffFacade.SetRightFilePath(p_RightFilePath);
}

bool Application::Run(bool p_bDoNotAsk)
{

  //
  // Opening the screen
  //
  m_Screen.SetTitle("ADiffView 1.0");

  if(m_PubScreenName.Length() > 0)
  {
    m_Screen.Open(AppScreen::SME_UseNamedPubScreen, m_PubScreenName);
  }
  else
  {
    m_Screen.Open(AppScreen::SME_CloneWorkbenchMin8Col);
  }

  if (!m_Screen.IsOpen())
  {
    // Opening the screen failed
    return false;
  }

  // TODO Debugging only. Remove afterwards.
  if((m_DiffFacade.LeftFilePath().Length() == 0) &&
     (m_DiffFacade.RightFilePath().Length() == 0))
  {
    m_DiffFacade.SetLeftFilePath("testfiles/Testcase_10_Left.txt");
    m_DiffFacade.SetRightFilePath("testfiles/Testcase_10_Right.txt");

    m_DiffFacade.SetLeftFilePath("S:Startup-Sequence-BB3.9-1");
    m_DiffFacade.SetRightFilePath("S:Startup-Sequence");

  }

  //
  // Filling the GadTools menu struct, supplying pointers to the
  // commands as nm_UserData. So no complicated evalution needed to
  // detect which menu item was clicked. Only the Execute() method
  // of the (then anonymous) command has to be called.
  //
  struct NewMenu menuDefinition[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {  NM_ITEM,   "Open files...",         "O", 0, 0, &m_CmdOpenFilesWindow },
//    {  NM_ITEM,   "Time statistics...",    "T", 0, 0, m_pCmdStatistics }, // TODO
    {  NM_ITEM,   NM_BARLABEL,              0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",                  "Q", 0, 0, &m_CmdQuit },
    {  NM_END,    NULL,                     0 , 0, 0, 0 },
  };

  //
  // Creating the menu
  //
  if(m_Menu.Create(menuDefinition) == false)
  {
    return false;
  }

  //
  // Installing menu to all windows
  //
  m_DiffWindow.SetMenu(&m_Menu);
  m_OpenFilesWin.SetMenu(&m_Menu);

  //
  // Prepare the DiffWindow
  //
  m_DiffWindow.SetInitialPosition(WindowBase::IP_Fill);
  m_DiffWindow.SetSmartRefresh(true);

  //
  // Open the OpenFilesWindow.
  //
  // Giving the command ptr as argument, so the appropriate menu item
  // is disabled after opening.
  m_OpenFilesWin.Open(&m_CmdOpenFilesWindow);
//m_DiffWindow.Open();

  // When DONOTASK argument is set and left and right file are passed,
  // start the diff immediately
  if((m_DiffFacade.LeftFilePath().Length() > 0) &&
     (m_DiffFacade.RightFilePath().Length() > 0) &&
     p_bDoNotAsk == true)
  {
    // TODO How can I do this?
  }

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
        struct MenuItem* pSelectedItem = ItemAddress(m_Menu.IntuiMenu(), menuNumber);

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
        if(m_DiffWindow.IsOpen() && msgWindow == m_DiffWindow.IntuiWindow())
        {
          m_DiffWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
        else if(m_OpenFilesWin.IsOpen() && msgWindow == m_OpenFilesWin.IntuiWindow())
        {
          m_OpenFilesWin.HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
      }

      if(!m_DiffWindow.IsOpen() && !m_OpenFilesWin.IsOpen())
      {
        // All windows are close: exit
        m_bExitRequested = true;
        break;
      }
    }
  }
  while(!m_bExitRequested);
}
