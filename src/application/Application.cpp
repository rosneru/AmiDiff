#include <stdio.h>

#include <libraries/gadtools.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/intuition.h>

#include "Command.h"
#include "MessageBox.h"
#include "WorkerProgressMsg.h"

#include "Application.h"

Application::Application(struct MsgPort* p_pMsgPortIDCMP,
                         struct MsgPort* p_pMsgPortProgress)
  : m_pMsgPortIDCMP(p_pMsgPortIDCMP),
    m_pMsgPortProgress(p_pMsgPortProgress),
    m_bCancelRequested(false),
    m_bExitRequested(false),
    m_NumWindowsOpen(0),
    m_bExitAllowed(true),
    m_Screen(),
    m_DiffWindow(m_Screen, m_pMsgPortIDCMP, m_NumWindowsOpen),
    m_FilesWindow(m_Screen, m_pMsgPortIDCMP, m_NumWindowsOpen,
                  m_LeftFilePath, m_RightFilePath, m_CmdDiff),
    m_ProgressWindow(m_Screen, m_pMsgPortIDCMP, m_NumWindowsOpen,
                     m_bCancelRequested),
    m_DiffWorker(m_LeftFilePath, m_RightFilePath, m_DiffWindow,
                 m_FilesWindow, m_ProgressWindow, p_pMsgPortProgress,
                 m_bCancelRequested, m_bExitAllowed),
    m_CmdDiff(m_DiffWorker),
    m_CmdQuit(m_bExitAllowed, m_bExitRequested),
    m_CmdOpenFilesWindow(m_FilesWindow),
    m_Menu(m_Screen)
{
}

Application::~Application()
{

}

void Application::SetLeftFilePath(const SimpleString& p_LeftFilePath)
{
  m_LeftFilePath = p_LeftFilePath;
}

void Application::SetRightFilePath(const SimpleString& p_RightFilePath)
{
  m_RightFilePath = p_RightFilePath;
}

void Application::SetPubScreenName(const SimpleString& p_PubScreenName)
{
  m_PubScreenName = p_PubScreenName;
}

bool Application::Run(bool p_bDoNotAsk)
{
  ::MessageBox request;

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
    request.Show("Error: Can't open screen.", "Ok");
    return false;
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
    request.Show("Error: Can't create the menu.", "Ok");
    return false;
  }

  //
  // Installing menu to all windows
  //
  m_DiffWindow.SetMenu(&m_Menu);
  m_FilesWindow.SetMenu(&m_Menu);

  //
  // Prepare the DiffWindow
  //
  m_DiffWindow.SetInitialPosition(WindowBase::IP_Fill);
  m_DiffWindow.SetSmartRefresh(true);


  if((m_LeftFilePath.Length() > 0) &&
     (m_RightFilePath.Length() > 0) &&
     p_bDoNotAsk == true)
  {
    //
    // The DONOTASK argument has been set and left and right file are
    // also given: Start the diff immediately
    //
    m_CmdDiff.Execute();
  }
  else
  {
    //
    // Default: Open the OpenFilesWindow.
    //
    // Giving the command ptr as argument, so the appropriate menu item
    // will be disabled after opening.
    m_FilesWindow.Open(&m_CmdOpenFilesWindow);
  }

  //
  // Wait-in-loop for menu actions etc
  //
  intuiEventLoop();

  return true;

}

void Application::intuiEventLoop()
{
  ULONG sigIDCMP = (1ul << m_pMsgPortIDCMP->mp_SigBit);
  ULONG sigProgress = (1ul << m_pMsgPortProgress->mp_SigBit);

  ULONG signals = sigIDCMP | sigProgress;

  do
  {
    const ULONG activeSignals = Wait(signals);

    if(activeSignals & sigProgress)
    {
      struct WorkerProgressMsg* pProgressMsg = NULL;
      while(pProgressMsg = (struct WorkerProgressMsg *)
              GetMsg(m_pMsgPortProgress))
      {
        if(m_ProgressWindow.IsOpen())
        {
          m_ProgressWindow.HandleProgress(pProgressMsg);
        }

        ReplyMsg(pProgressMsg);
      }
    }

    if(activeSignals & sigIDCMP)
    {
      struct IntuiMessage* pIdcmpMsg = NULL;
      while (pIdcmpMsg = (struct IntuiMessage *)
               GT_GetIMsg(m_pMsgPortIDCMP))
      {
        // Get all data we need from message
        ULONG msgClass = pIdcmpMsg->Class;
        UWORD msgCode = pIdcmpMsg->Code;
        APTR msgIAddress = pIdcmpMsg->IAddress;
        struct Window* msgWindow = pIdcmpMsg->IDCMPWindow;

        // When we're through with a message, reply
        GT_ReplyIMsg(pIdcmpMsg);

        if(msgClass == IDCMP_MENUPICK)
        {
          //
          // Menu-pick messages are handled here
          //
          UWORD menuNumber = msgCode;
          struct MenuItem* pSelectedItem = ItemAddress(
            m_Menu.IntuiMenu(), menuNumber);

          if(pSelectedItem != NULL)
          {
            // Getting the user data from selected menu item
            APTR pUserData = GTMENUITEM_USERDATA(pSelectedItem);
            if(pUserData != NULL)
            {
              // The menu user_data contains a pointer to a Command
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
          if(m_DiffWindow.IsOpen() &&
             msgWindow == m_DiffWindow.IntuiWindow())
          {
            m_DiffWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
          }
          else if(m_FilesWindow.IsOpen() &&
                  msgWindow == m_FilesWindow.IntuiWindow())
          {
            m_FilesWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
          }
          else if(m_ProgressWindow.IsOpen() &&
                  msgWindow == m_ProgressWindow.IntuiWindow())
          {
            m_ProgressWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
          }
        }
      }
    }

    if(m_NumWindowsOpen < 1)
    {
      m_CmdQuit.Execute();
    }
  }
  while(!m_bExitRequested);
}
