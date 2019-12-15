#include "MessageBox.h"
#include "CmdAbout.h"

CmdAbout::CmdAbout(AppScreen& screen,
                   AppMenu& menu,
                   const char* pVersTag)
  : m_Screen(screen),
    m_Menu(menu)
{
  m_AboutMsg = pVersTag + 7;  // Skip the first 7 chars of pVersTag
                              // which is only "\0$VER: "
  m_AboutMsg += "\n\n";
  m_AboutMsg += "Copyright(c) 2019 Uwe Rosner (u.rosner@ymail.com)";
  m_AboutMsg += "\n\n";
  m_AboutMsg += "This release of ADiffView may be freely distributed.\n";
  m_AboutMsg += "It may not be comercially distributed without the\n";
  m_AboutMsg += "explicit permission of the author.\n";
}

CmdAbout::~CmdAbout()
{
}

void CmdAbout::Execute() const
{
  // Get the active window
  struct Window* pActiveWindow = m_Screen.ActiveWindow();

  // Disable this command in the menu of the active window
  m_Menu.DisableMenuItem(pActiveWindow, (APTR)this);

  // Display the about dialog
  MessageBox messageBox;
  messageBox.Show(pActiveWindow, "About", m_AboutMsg.C_str(), "Ok");

  // Enable this command in the menu of the active window
  m_Menu.EnableMenuItem(pActiveWindow, (APTR)this);
}
