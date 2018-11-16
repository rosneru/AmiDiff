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

Application::Application(int argc, char **argv, struct MsgPort* p_pMsgPortAllWindows)
  : m_pMsgPortAllWindows(p_pMsgPortAllWindows),
    m_Argc(argc),
    m_Argv(argv),
    m_bExitRequested(false),
    m_Screen("AmiDiff (C) 2018 by Uwe Rosner."),
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
  //
  // Ensure that all windows are closed before the screen is closed
  //

  if(m_OpenFilesWin.IsOpen())
  {
    m_OpenFilesWin.Close();
  }

  if(m_DiffWindow.IsOpen())
  {
    m_DiffWindow.Close();
  }

  m_Screen.Close();

}


bool Application::Run()
{

  //
  // Opening the screen
  //
  if (!m_Screen.Open())
  {
    // Opening the screen failed
    return false;
  }


  //
  // If there are at least two command line arguments permitted, take
  // the first two of them (argv[1] and argv[2]  as file names and load
  // them into left and right window.
  //
  if(m_Argc >= 3)
  {
    m_DiffFacade.SetLeftFilePath(m_Argv[1]);
    m_DiffFacade.SetRightFilePath(m_Argv[2]);
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
  // Prepare the left and right TextWindows and open the OpenFileWindow
  //
  m_DiffWindow.SetInitialPosition(WindowBase::IP_Fill);
  m_DiffWindow.Open();

  //m_OpenFilesWin.Open(&m_CmdOpenFilesWindow);

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
