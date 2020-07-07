#include "MessageBox.h"
#include "CmdAbout.h"


CmdAbout::CmdAbout(Array<WindowBase*>& windowArray,
                   const char* pVersTag)
  : CommandBase(windowArray)
{
  m_AboutMsg = pVersTag + 7;  // Skip the first 7 chars of pVersTag
                              // which is only "\0$VER: "
  m_AboutMsg += "\n\n";
  m_AboutMsg += "Copyright(c) 2020 Uwe Rosner (u.rosner@ymail.com)";
  m_AboutMsg += "\n\n";
  m_AboutMsg += "This release of ADiffView may be freely distributed.\n";
  m_AboutMsg += "It may not be comercially distributed without the\n";
  m_AboutMsg += "explicit permission of the author.\n";
}

CmdAbout::~CmdAbout()
{
}

void CmdAbout::Execute(struct Window* pActiveWindow)
{
  // Disable the "About..." item (this command) in all menus
  DisableInAllWindowMenus();

  // Set the busy pointer for all windows
  enableBusyPointerForAllWindows();

  // Display the about dialog
  MessageBox messageBox;
  messageBox.Show(pActiveWindow, "About", m_AboutMsg.C_str(), "Ok");

  // Set the normal pointer for all windows
  disableBusyPointerForAllWindows();

  // Enable the "About..." item (this command) in all menus
  EnableInAllWindowMenus();
}
