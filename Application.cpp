#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include "LinkedList.h"

#include "Command.h"
#include "CmdPerformDiff.h"
#include "CmdOpenWindow.h"
#include "CmdQuit.h"

#include "TextDocument.h"

#include "Application.h"

Application::Application(int argc, char **argv)
  : m_Argc(argc),
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

  if(m_pDiffFacade != NULL)
  {
    delete m_pDiffFacade;
    m_pDiffFacade = NULL;
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
  // Creating the DiffFacade TODO
  //
  m_pDiffFacade = new AmigaDiffFacade();



  //
  // Creating left and right diff windows but not opening them yet
  //
  m_pLeftWin = new TextWindow(m_pScreen);
  m_pRightWin = new TextWindow(m_pScreen);

  //
  // Instantiating the commands
  //
  m_pCmdQuit = new CmdQuit(m_bExitRequested);
  m_pCmdDiff = new CmdPerformDiff(*m_pDiffFacade);

  //
  // Now that the CmdPerformDiff is available the OpenFilesWindow can
  // be  created.
  //
  m_pOpenFilesWin = new OpenFilesWindow(m_pScreen, m_LeftFilePath,
    m_RightFilePath, *m_pCmdDiff);

  m_pCmdOpenFilesWindow = new CmdOpenWindow(*m_pOpenFilesWin);
                  // TODO How can the "Open..." in newMenuDefinition be
                  // disabled if the OpenFilesWindow is already open.
                  // Or can we "extend" the OpenCmd to bring the window
                  // in front then?


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
//    {  NM_ITEM,   "Time statistics...",    "T", 0, 0, m_pCmdDiff }, // TODO
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

  //
  // If there are at least two command line arguments permitted,
  // (three for the if as the first one is the application name),
  // try to take the first two of them as file names and load them
  // into left and right window
  //
  if(m_Argc >= 3)
  {
    m_LeftFilePath = m_Argv[1];
    m_RightFilePath = m_Argv[2];
  }

//  m_pOpenFilesWin->Open();
  m_pLeftWin->Open();

  //
  // Wait-in-loop for menu actions etc
  //
  intuiEventLoop();

  return true;

}

ULONG Application::signalMask()
{
  ULONG signal = 0;

  if(m_pLeftWin != NULL && m_pLeftWin->IntuiWindow() != NULL)
  {
    signal |= 1L << m_pLeftWin->IntuiWindow()->UserPort->mp_SigBit;
  }

  if(m_pRightWin != NULL && m_pRightWin->IntuiWindow() != NULL)
  {
    signal |= 1L << m_pRightWin->IntuiWindow()->UserPort->mp_SigBit;
  }

  if(m_pOpenFilesWin != NULL && m_pOpenFilesWin->IntuiWindow() != NULL)
  {
    signal |= 1L << m_pOpenFilesWin->IntuiWindow()->UserPort->mp_SigBit;
  }

  return signal;
}

struct IntuiMessage* Application::nextIntuiMessage()
{
  struct IntuiMessage* pIntuiMessage = NULL;

  // Look for message in userport of left window
  if(m_pLeftWin->IntuiWindow() != NULL)
  {
    pIntuiMessage = (struct IntuiMessage *)
      GetMsg(m_pLeftWin->IntuiWindow()->UserPort);

    if(pIntuiMessage != NULL)
    {
      return pIntuiMessage;
    }
  }

  // Look for message in userport of right window
  if(m_pRightWin->IntuiWindow() != NULL)
  {
    pIntuiMessage = (struct IntuiMessage *)
      GetMsg(m_pRightWin->IntuiWindow()->UserPort);

    if(pIntuiMessage != NULL)
    {
      return pIntuiMessage;
    }
  }

    // Look for message in userport of open files window
  if(m_pOpenFilesWin->IntuiWindow() != NULL)
  {
    pIntuiMessage = (struct IntuiMessage *)
      GetMsg(m_pOpenFilesWin->IntuiWindow()->UserPort);

    if(pIntuiMessage != NULL)
    {
      return pIntuiMessage;
    }
  }

  return NULL;
}

void Application::intuiEventLoop()
{
  ULONG mask = (1L << m_pLeftWin->IntuiWindow()->UserPort->mp_SigBit);
  struct IntuiMessage* pMsg;
  do
  {
    Wait(mask);
    while (pMsg = (struct IntuiMessage *)GetMsg(m_pLeftWin->IntuiWindow()->UserPort))
    {
      // Get all data we need from message
      ULONG msgClass = pMsg->Class;
      UWORD msgCode = pMsg->Code;
      APTR msgIAddress = pMsg->IAddress;
      struct Window* msgWindow = pMsg->IDCMPWindow;

      // When we're through with a message, reply
      ReplyMsg((struct Message *)pMsg);

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
        m_pLeftWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
      }

      if(!m_pLeftWin->IsOpen())
      {
        m_bExitRequested = true;
        break;
      }
    }
  }
  while(!m_bExitRequested);
}
/*
void Application::intuiEventLoop()
{
  struct Window* pWin1 = m_pLeftWin->IntuiWindow();
  struct Window* pWin2 = m_pRightWin->IntuiWindow();
  struct Window* pWin3 = m_pOpenFilesWin->IntuiWindow();

  struct Menu* pMenu = m_pMenu->IntuiMenu();

  while((pWin1 != NULL || pWin2 != NULL || pWin3 != NULL) &&
        m_bExitRequested == false)
  {
    // Waiting for messages from Intuition
    ULONG mask = 0;
    if(m_pLeftWin != NULL && m_pLeftWin->IntuiWindow() != NULL)
    {
      mask |= 1L << m_pLeftWin->IntuiWindow()->UserPort->mp_SigBit;
    }

    if(m_pRightWin != NULL && m_pRightWin->IntuiWindow() != NULL)
    {
      mask |= 1L << m_pRightWin->IntuiWindow()->UserPort->mp_SigBit;
    }

    if(m_pOpenFilesWin != NULL && m_pOpenFilesWin->IntuiWindow() != NULL)
    {
      mask |= 1L << m_pOpenFilesWin->IntuiWindow()->UserPort->mp_SigBit;
    }

    Wait(mask);

    struct IntuiMessage* pMsg;

    while ((pWin1 != NULL) && (pMsg = (struct IntuiMessage *)GetMsg(pWin1->UserPort)) ||
           (pWin2 != NULL) && (pMsg = (struct IntuiMessage *)GetMsg(pWin2->UserPort)) ||
           (pWin3 != NULL) && (pMsg = (struct IntuiMessage *)GetMsg(pWin3->UserPort)))
    {
      // Get all data we need from message
      ULONG msgClass = pMsg->Class;
      UWORD msgCode = pMsg->Code;
      APTR msgIAddress = pMsg->IAddress;
      struct Window* msgWindow = pMsg->IDCMPWindow;

      // When we're through with a message, reply
      ReplyMsg((struct Message *)pMsg);

      if(msgClass == IDCMP_MENUPICK)
      {
        //
        // Menupick messages are handled here
        //
        UWORD menuNumber = msgCode;
        struct MenuItem* pSelectedItem = ItemAddress(pMenu, menuNumber);

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
        if(msgWindow == m_pLeftWin->IntuiWindow())
        {
          m_pLeftWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
        else if(msgWindow == m_pRightWin->IntuiWindow())
        {
          m_pRightWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
        else if(msgWindow == m_pOpenFilesWin->IntuiWindow())
        {
          m_pOpenFilesWin->HandleIdcmp(msgClass, msgCode, msgIAddress);
        }
      }

      pWin1 = m_pLeftWin->IntuiWindow();
      pWin2 = m_pRightWin->IntuiWindow();
      pWin3 = m_pOpenFilesWin->IntuiWindow();

      if(pWin1 == NULL && pWin2 == NULL && pWin3 == NULL)
      {
        // Exit because all windows are closed
        m_bExitRequested = true;
      }
    }
  }
}
*/
