#include "MessageBox.h"
#include "CmdAbout.h"

CmdAbout::CmdAbout(AppScreen& p_Screen, ApplicationMenu& p_Menu)
  : m_Screen(p_Screen),
    m_Menu(p_Menu)
{
  m_AboutMsg  = "ADiffView 1.0.1 (13.03.2019)\n";
  m_AboutMsg += "\n";
  m_AboutMsg += "Autor: Uwe Rosner (u.rosner@ymail.com)\n";
  m_AboutMsg += "\n";
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
