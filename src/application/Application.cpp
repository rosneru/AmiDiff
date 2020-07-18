#include <stdio.h>

#include <libraries/gadtools.h>
#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>
#include <workbench/workbench.h>

#include "ADiffView_rev.h"
#include "CommandBase.h"
#include "ProgressMessage.h"

#include "Application.h"

Application::Application(ScreenBase* pScreenBase, 
                         ADiffViewArgs& args, 
                         ADiffViewSettings& settings)
  : m_Args(args),
    m_LeftFilePath(args.LeftFile()),   // copy (not reference) to member
    m_RightFilePath(args.RightFile()), // copy (not reference) to member
    m_Settings(settings),
    m_pMsgPortIDCMP(NULL),
    m_pMsgPortProgress(NULL),
    m_pMsgPortAppWindow(NULL),
    m_bCancelRequested(false),
    m_bExitRequested(false),
    m_bExitAllowed(true),
    m_DiffWorker(m_LeftFilePath,
                 m_RightFilePath,
                 m_DiffWindow,
                 m_ProgressWindow,
                 m_CmdOpenFilesWindow,
                 m_CmdCloseFilesWindow,
                 m_pMsgPortProgress,
                 m_bCancelRequested,
                 m_bExitAllowed),
    m_pScreenBase(pScreenBase),
    m_DiffWindow(pScreenBase,
                 m_pMsgPortIDCMP,
                 &m_DiffWindowMenu),
    m_FilesWindow(m_WindowArray,
                  pScreenBase,
                  m_pMsgPortIDCMP,
                  m_LeftFilePath,
                  m_RightFilePath,
                  m_CmdDiff,
                  m_CmdCloseFilesWindow,
                  &m_FilesWindowMenu),
    m_ProgressWindow(pScreenBase,
                     m_pMsgPortIDCMP,
                     m_bCancelRequested,
                     NULL),
    m_CmdDiff(&m_WindowArray, m_DiffWorker),
    m_CmdNavNextDiff(&m_WindowArray,m_DiffWindow),
    m_CmdNavPrevDiff(&m_WindowArray,m_DiffWindow),
    m_CmdQuit(&m_WindowArray, m_bExitAllowed, m_bExitRequested),
    m_CmdOpenFilesWindow(&m_WindowArray, m_FilesWindow),
    m_CmdCloseFilesWindow(&m_WindowArray, m_CmdOpenFilesWindow, m_FilesWindow),
    m_CmdAboutRequester(&m_WindowArray, m_AboutMsg, "About", "Ok")
{
  //
  // Note: VERSTAG above has been created with bumprev and is defined
  // in included file ADiffView_rev.h. It contains the program name
  // and version. Referencing it also means that the c:version command will
  // work for ADiffView.
  //

  // Create the message to be displayed in the about dialog
  m_AboutMsg = VERSTAG + 7;   // Skip the first 7 chars of pVersTag
                              // which is only "\0$VER: "
  m_AboutMsg += "\n\n";
  m_AboutMsg += "Copyright(c) 2020 Uwe Rosner (u.rosner@ymail.com)";
  m_AboutMsg += "\n\n";
  m_AboutMsg += "This release of ADiffView may be freely distributed.\n";
  m_AboutMsg += "It may not be comercially distributed without the\n";
  m_AboutMsg += "explicit permission of the author.\n";


  // Add all windows to the array
  m_WindowArray.push_back(&m_DiffWindow);
  m_WindowArray.push_back(&m_FilesWindow);
  m_WindowArray.push_back(&m_ProgressWindow);

  //
  // Message port initialization
  //

  // Create a message port for all windows (IDCMP) messages
  m_pMsgPortIDCMP = CreateMsgPort();
  if(m_pMsgPortIDCMP == NULL)
  {
    throw "Failed to create the windows message port.";
  }

  // Create a message port for progress notification
  m_pMsgPortProgress = CreateMsgPort();
  if(m_pMsgPortProgress == NULL)
  {
    throw "Failed to create the progress message port.";
  }

  // If running on Workbench screen, set OpenFilesWindow as an AppWindow
  bool bFilesWindowIsAppWindow = (m_Args.PubScreenName() == "Workbench") ||
                                 (m_Args.AskOnWorkbench() == true);
  if(bFilesWindowIsAppWindow)
  {
    // Create a message port for the AppWindow
    m_pMsgPortAppWindow = CreateMsgPort();
    if(m_pMsgPortAppWindow == NULL)
    {
      throw "Failed to create the app window message port.";
    }

    // FilesWindow should be an AppWindow
    m_FilesWindow.EnableAppWindow(m_pMsgPortAppWindow, 1); // TODO Avoid numeric constant
  }

  // Apply some settings and arguments
  m_DiffWorker.SetLineNumbers(m_Args.ShowLineNumbers());

  //
  // Fill the GadTools menu structs, supplying pointers to the commands
  // as user data. So in event loop no complicated evaluation is needed
  // to detect which menu item was selected. It will be a command, and
  // only its Execute() method must be called.
  //
  struct NewMenu aboutWinNewMenu[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {   NM_ITEM,    "Open...",             "O", 0, 0, &m_CmdOpenFilesWindow },
    {   NM_ITEM,    "About...",             0 , 0, 0, &m_CmdAboutRequester },
    {   NM_ITEM,    NM_BARLABEL,            0 , 0, 0, 0 },
    {   NM_ITEM,    "Quit",                "Q", 0, 0, &m_CmdQuit },
    { NM_END,     NULL,                     0 , 0, 0, 0 },
  };

  size_t aboutWinMenuSize = sizeof(aboutWinNewMenu) / sizeof(aboutWinNewMenu[0]);
  m_FilesWindowMenu.SetMenuDefinition(aboutWinNewMenu, aboutWinMenuSize);
  m_FilesWindow.SetMenu(&m_FilesWindowMenu);

  struct NewMenu diffWinNewMenu[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {   NM_ITEM,    "Open...",             "O", 0, 0, &m_CmdOpenFilesWindow },
    {   NM_ITEM,    "About...",             0 , 0, 0, &m_CmdAboutRequester },
    {   NM_ITEM,    NM_BARLABEL,            0 , 0, 0, 0 },
    {   NM_ITEM,    "Quit",                "Q", 0, 0, &m_CmdQuit },
    { NM_TITLE,   "Navigate",               0 , 0, 0, 0 },
    {   NM_ITEM,    "Previous difference", "P", 0, 0, &m_CmdNavPrevDiff },
    {   NM_ITEM,    "Next difference",     "N", 0, 0, &m_CmdNavNextDiff },
    { NM_END,     NULL,                     0 , 0, 0, 0 },
  };

  size_t diffWinMenuSize = sizeof(diffWinNewMenu) / sizeof(diffWinNewMenu[0]);
  m_DiffWindowMenu.SetMenuDefinition(diffWinNewMenu, diffWinMenuSize);
  m_DiffWindow.SetMenu(&m_DiffWindowMenu);

  if((m_LeftFilePath.Length() > 0) &&
     (m_RightFilePath.Length() > 0) &&
     (m_Args.DontAsk() == true))
  {
    //
    // The DONTASK argument has been set and left and right file are
    // also given: Start the diff immediately
    //
    m_CmdDiff.Execute(NULL);
  }
  else
  {
    //
    // Fallback / default: Open the FilesWindow
    //
    // Passing a pointer to the command "Open files" to disable that
    // menu item at opening.
    m_CmdOpenFilesWindow.Execute(NULL);
  }
}


Application::~Application()
{
  // Ensure that all windows are closed before deleting the message
  // ports
  m_DiffWindow.Close();
  m_FilesWindow.Close();
  m_ProgressWindow.Close();

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

  if(m_pMsgPortAppWindow!= NULL)
  {
    DeleteMsgPort(m_pMsgPortAppWindow);
    m_pMsgPortAppWindow = NULL;
  }
}


bool Application::Run()
{
  //
  // The main event loop
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

  ULONG sigAppWin = 0;
  if(m_pMsgPortAppWindow != NULL)
  {
    sigAppWin = (1ul << m_pMsgPortAppWindow->mp_SigBit);
  }

  ULONG signals = sigIDCMP | sigProgress | sigAppWin;

  do
  {
    const ULONG received = Wait(signals);

    if(received & sigAppWin)
    {
      handleAppWindowMessages();
    }

    if(received & sigProgress)
    {
      handleProgressMessages();
    }

    if(received & sigIDCMP)
    {
      handleIdcmpMessages();
    }

    // Check all screens for any open windows
    if(m_pScreenBase->NumOpenWindows() < 1)
    {
      // Exitting as there are no windows open anymore.
      m_CmdQuit.Execute(NULL); // Only sets m_bExitRequested to true.
    }
  }
  while(!m_bExitRequested);
}


void Application::handleAppWindowMessages()
{
  struct AppMessage* pAppMsg = NULL;
  while((pAppMsg = (struct AppMessage*)
    GetMsg(m_pMsgPortAppWindow)) != NULL)
  {
    m_FilesWindow.HandleAppMessage(pAppMsg);

    // All messages must be replied
    ReplyMsg((struct Message*)pAppMsg);
  }
}


void Application::handleProgressMessages()
{
  struct ProgressMessage* pProgressMsg = NULL;
  while((pProgressMsg = (struct ProgressMessage*)
    GetMsg(m_pMsgPortProgress)) != NULL)
  {
    m_ProgressWindow.HandleProgress(pProgressMsg);

    // All messages must be replied
    ReplyMsg(pProgressMsg);
  }
}


void Application::handleIdcmpMessages()
{
  struct IntuiMessage* pIdcmpMsg = NULL;
  while ((pIdcmpMsg = GT_GetIMsg(m_pMsgPortIDCMP)) != NULL)
  {
    // Get all data we need from message
    ULONG msgClass = pIdcmpMsg->Class;
    UWORD msgCode = pIdcmpMsg->Code;
    APTR msgIAddress = pIdcmpMsg->IAddress;
    struct Window* pMsgWindow = pIdcmpMsg->IDCMPWindow;

    // When we're through with a message, reply
    GT_ReplyIMsg(pIdcmpMsg);

    if(msgClass == IDCMP_MENUPICK)
    {
      //
      // Menu-pick messages are handled here
      //
      UWORD menuNumber = msgCode;
      struct MenuItem* pSelectedItem = NULL;

      // Create an array of all menus to be searched for the item
      AMenu* pMenus[] = {&m_FilesWindowMenu, &m_DiffWindowMenu};

      // Iterate all those menus, trying to find the item
      for(size_t i = 0; i < (sizeof pMenus / sizeof pMenus[0]); i++)
      {
        pSelectedItem = ItemAddress(pMenus[i]->IntuiMenu(), menuNumber);
        if(pSelectedItem != NULL)
        {
          // Item found
          break;
        }
      }

      if(pSelectedItem != NULL)
      {
        // Getting the user data from selected menu item
        APTR pUserData = GTMENUITEM_USERDATA(pSelectedItem);
        if(pUserData != NULL)
        {
          // The menu user_data contains a pointer to a Command
          CommandBase* pSelecedCommand = static_cast<CommandBase*>(pUserData);

          // Execute this command
          pSelecedCommand->Execute(pMsgWindow);
        }
      }

      return;
    }

    //
    // All other idcmp messages are handled in the appropriate window
    //
    if(pMsgWindow == m_DiffWindow.IntuiWindow())
    {
      m_DiffWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
    }
    else if(pMsgWindow == m_FilesWindow.IntuiWindow())
    {
      m_FilesWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
    }
    else if(pMsgWindow == m_ProgressWindow.IntuiWindow())
    {
      m_ProgressWindow.HandleIdcmp(msgClass, msgCode, msgIAddress);
    }
  }
}
