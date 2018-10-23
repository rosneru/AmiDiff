#include <libraries/gadtools.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>

#include "LinkedList.h"

#include "Command.h"
#include "CmdOpenFilesWindow.h"
#include "CmdQuit.h"

#include "TextDocument.h"

#include "Application.h"

Application::Application(int argc, char **argv)
  : m_Argc(argc),
    m_Argv(argv),
    m_pScreen(NULL),
    m_pOpenFilesWindow(NULL),
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
    //m_pRightWin->Close();
    delete m_pRightWin;
    m_pRightWin = NULL;
  }

  if(m_pLeftWin != NULL)
  {
    //m_pLeftWin->Close();
    delete m_pLeftWin;
    m_pLeftWin = NULL;
  }

  if(m_pOpenFilesWindow != NULL)
  {
    //m_pOpenFilesWindow->Close();
    delete m_pOpenFilesWindow;
    m_pOpenFilesWindow = NULL;
  }

  if(m_pScreen != NULL)
  {
    //m_pScreen->Close();
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
  // Creating the window to open the files to diff. Note: It's not
  // getting opened yet. It will be opened later by CmdOpenFilesWindow.
  //
  m_pOpenFilesWindow = new OpenFilesWindow(m_pScreen, m_LeftFilePath,
    m_RightFilePath);

  //
  // Opening the left window
  //
  m_pLeftWin = new TextWindow(m_pScreen);
  if(!m_pLeftWin->Open()) // TODO make it DiffWindow and give DiffWindow::LEFT
  {
    // Opening the window failed
    Dispose();
    return false;
  }

  //
  // Opening the right window
  //
  m_pRightWin = new TextWindow(m_pScreen);
  if(!m_pRightWin->Open()) // TODO make it DiffWindow and give DiffWindow::RIGHT
  {
    // Opening the window failed
    Dispose();
    return false;
  }

  //
  // Instantiating the commands
  //
  m_pCmdQuit = new CmdQuit(m_bExitRequested);

//  m_pCmdOpenLeftFile = new CmdOpenFilesWindow(*m_pOpenFilesWindow);
                  // TODO How can the "Open..." in newMenuDefinition be
                  // disabled if the OpenFilesWindow is already open.
                  // Or can we "extend" the OpenCmd to bring the window
                  // in front then?

//  TextDocument rightDocument;
//  m_pCmdOpenRightFile = new CmdFileOpen("Open right file", rightDocument, *m_pRightWin);


  //
  // Fill the GadTools menu struct, supplying pointers to the commands
  // as nm_UserData. With this behavior there is no complicated
  // evalution needed to detect which menu item was clicked. Just
  // the ->Execute of the (by then anonymous) command has to be called.
  //
  struct NewMenu menuDefinition[] =
  {
    { NM_TITLE,   "Project",                0 , 0, 0, 0 },
    {  NM_ITEM,   "Open...",               "O", 0, 0, m_pCmdQuit },
//    {  NM_ITEM,   "Time statistics...",    "T", 0, 0, m_pCmdOpenRightFile }, // TODO
    {  NM_ITEM,   NM_BARLABEL,              0 , 0, 0, 0 },
    {  NM_ITEM,   "Quit",                  "Q", 0, 0, m_pCmdQuit },
    {  NM_END,    NULL,                     0 , 0, 0, 0 },
  };

  //
  // Creating the menu
  //
  m_pMenu = new AppMenu(m_pScreen);
  if(m_pMenu->Create(menuDefinition) == FALSE)
  {
    Dispose();
    return false;
  }

  //
  // Installing menu to all windows
  //
  if(m_pMenu->AttachToWindow(m_pLeftWin) == FALSE)
  {
    Dispose();
    return false;
  }

  if(m_pMenu->AttachToWindow(m_pRightWin) == FALSE)
  {
    Dispose();
    return false;
  }


  if(m_pMenu->AttachToWindow(m_pOpenFilesWindow) == FALSE)
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
    m_LeftFilePath = m_Argv[1];
    m_RightFilePath = m_Argv[2];
  }

  //
  // Wait-in-loop for menu actions etc
  //
  intuiEventLoop();

  return true;

}

void Application::intuiEventLoop()
{
  //
  // Waiting for messages from Intuition
  //

  struct Window* pWin1 = m_pLeftWin->IntuiWindow();
  struct Window* pWin2 = m_pRightWin->IntuiWindow();
  struct Window* pWin3 = m_pOpenFilesWindow->IntuiWindow();
  struct Menu* pMenu = m_pMenu->IntuiMenu();


  while (m_bExitRequested == false)
  {
    // Waiting for a signals from the windows
    Wait(1L << pWin1->UserPort->mp_SigBit |
         1L << pWin2->UserPort->mp_SigBit |
         1L << pWin3->UserPort->mp_SigBit);

    struct IntuiMessage* pMsg;
    while ((m_bExitRequested == false) &&
          ((pMsg = (struct IntuiMessage *)GetMsg(pWin1->UserPort)) ||
           (pMsg = (struct IntuiMessage *)GetMsg(pWin2->UserPort)) ||
           (pMsg = (struct IntuiMessage *)GetMsg(pWin3->UserPort)) ))
    {
      if(pMsg->Class == IDCMP_MENUPICK)
      {
        //
        // Menupick messages are handled here
        //
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
      }
      else
      {
        //
        // All other messages are handled in the appropriate window
        //
        if(pMsg->IDCMPWindow == m_pLeftWin->IntuiWindow())
        {
          m_pLeftWin->HandleIdcmp(pMsg);
        }
        else if(pMsg->IDCMPWindow == m_pRightWin->IntuiWindow())
        {
          m_pRightWin->HandleIdcmp(pMsg);
        }
        else if(pMsg->IDCMPWindow == m_pOpenFilesWindow->IntuiWindow())
        {
          m_pOpenFilesWindow->HandleIdcmp(pMsg);
        }
      }

      //
      // Every IntuiMessage has to be replied
      //
      ReplyMsg((struct Message *)pMsg);
    }
  }
}
