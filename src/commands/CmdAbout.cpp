#include "MessageBox.h"
#include "CmdAbout.h"

CmdAbout::CmdAbout()
{
  m_AboutMsg  = "ADiffView 1.0 (28.02.2019)\n";
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
  MessageBox messageBox;
  messageBox.Show(m_AboutMsg.C_str(), "Ok");
}
