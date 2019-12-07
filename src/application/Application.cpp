#include <stdio.h>

#include <libraries/gadtools.h>
#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>

#include "Command.h"
#include "MessageBox.h"
#include "ProgressMessage.h"

#include "Application.h"

Application::Application(ADiffViewOptions& options)
  : m_Options(options),
    m_LeftFilePath(options.LeftFile()),   // ceate copy in member
    m_RightFilePath(options.RightFile()), // ceate copy in member
    m_pMsgPortIDCMP(NULL),
    m_pMsgPortProgress(NULL),
    m_NumWindowsOpen(0),
    m_bCancelRequested(false),
    m_bExitRequested(false),
    m_bExitAllowed(true),
    m_Screen(),
    m_Menu(m_Screen),
    m_DiffWindow(m_Screen,
                 m_pMsgPortIDCMP,
                 m_NumWindowsOpen),
    m_FilesWindow(m_Screen,
                  m_pMsgPortIDCMP,
                  m_NumWindowsOpen,
                  m_LeftFilePath,
                  m_RightFilePath,
                  m_CmdDiff),
    m_ProgressWindow(m_Screen,
                     m_pMsgPortIDCMP,
                     m_NumWindowsOpen,
                     m_bCancelRequested),
    m_DiffWorker(m_LeftFilePath,
                 m_RightFilePath,
                 m_DiffWindow,
                 m_FilesWindow,
                 m_ProgressWindow,
                 m_pMsgPortProgress,
                 m_bCancelRequested,
                 m_bExitAllowed),
    m_CmdDiff(m_DiffWorker),
    m_CmdQuit(m_bExitAllowed,
              m_bExitRequested),
    m_CmdOpen(m_FilesWindow),
    m_CmdAbout(m_Screen,
               m_Menu)
{
}

Application::~Application()
{
  if(m_pMsgPortProgress != NULL)
  {
    DeleteMsgPort(m_pMsgPortProgress);
    m_pMsgPortProgress = NULL;
  }

  if(m_pMsgPortIDCMP != NULL)
  {
    DeleteMsgPort(m_pMsgPortIDCMP);
    m_pMsgPortIDCMP = NULL;
  }
}


bool Application::Run()
{
  //
  // Message port initialization
  //

  // Create a message port for all windows' IDCMP messages
  m_pMsgPortIDCMP = CreateMsgPort();
  if(m_pMsgPortIDCMP == NULL)
  {
    m_ErrorMsg = "Error: Can't create idcmp message port.";
    return false;
  }

  // Create a message port for progress notification
  m_pMsgPortProgress = CreateMsgPort();
  if(m_pMsgPortProgress == NULL)
  {
    m_ErrorMsg = "Error: Can't create progress message port.";
    return false;
  }


  //
  // Open the screen
  //
  m_Screen.SetTitle("ADiffView 1.0");

  if(m_Options.PubScreenName().Length() > 0)
  {
    m_Screen.Open(AppScreen::SME_UseNamedPubScreen,
                  m_Options.PubScreenName());
  }
  else
  {
    m_Screen.Open(AppScreen::SME_CloneWorkbenchMin8Col);
  }

  if (!m_Screen.IsOpen())
  {
    // Opening the screen failed
    m_ErrorMsg = "Error: Can't open screen.";
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
    {  NM_TITLE,  "Project",      0 , 0, 0, 0 },
    {  NM_ITEM,   "Open...",     "O", 0, 0, &m_CmdOpen },
    {  NM_ITEM,   "About...",     0 , 0, 0, &m_CmdAbout },
    {  NM_ITEM,   NM_BARLABEL,    0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",        "Q", 0, 0, &m_CmdQuit },
    {  NM_END,    NULL,           0 , 0, 0, 0 },
  };

  //
  // Creating the menu
  //
  if(m_Menu.Create(menuDefinition) == false)
  {
    m_ErrorMsg = "Error: Can't create the menu.";
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
  m_DiffWindow.SetSmartRefresh(true);


  if((m_LeftFilePath.Length() > 0) &&
     (m_RightFilePath.Length() > 0) &&
     (m_Options.DontAsk() == true))
  {
    //
    // The DONTASK argument has been set and left and right file are
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
    m_FilesWindow.Open(&m_CmdOpen);
  }

  //
  // The Amiga Intuition event loop
  //
  intuiEventLoop();

  return true;

}

SimpleString& Application::ErrorMsg()
{
  return m_ErrorMsg;
}

void Application::intuiEventLoop()
{
  ULONG sigIDCMP = (1ul << m_pMsgPortIDCMP->mp_SigBit);
  ULONG sigProgress = (1ul << m_pMsgPortProgress->mp_SigBit);

  ULONG signals = sigIDCMP | sigProgress;

  do
  {
    const ULONG received = Wait(signals);

    if(received & sigProgress)
    {
      struct ProgressMessage* pProgressMsg = NULL;
      while(pProgressMsg = (struct ProgressMessage *)
              GetMsg(m_pMsgPortProgress))
      {
        if(m_ProgressWindow.IsOpen())
        {
          m_ProgressWindow.HandleProgress(pProgressMsg);
        }

        // When we're through with a message, reply
        ReplyMsg(pProgressMsg);
      }
    }

    if(received & sigIDCMP)
    {
      struct IntuiMessage* pIdcmpMsg = NULL;
      while (pIdcmpMsg = GT_GetIMsg(m_pMsgPortIDCMP))
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
      m_CmdQuit.Execute(); // Only sets m_bExitRequested to true.
                           // But can also be executed from menu
                           // or Cancel button of FilesWindow.
    }
  }
  while(!m_bExitRequested);
}
