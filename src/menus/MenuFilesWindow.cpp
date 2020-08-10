#include <clib/gadtools_protos.h>

#include "MenuFilesWindow.h"

MenuFilesWindow::MenuFilesWindow(CommandBase* pCmdOpenFilesWindow,
                                 CommandBase* pCmdAboutRequester,
                                 CommandBase* pCmdQuit)
{
  struct NewMenu newMenu[] = 
  {
    { NM_TITLE,   "Project",                 0 , 0, 0, 0 },
    {   NM_ITEM,    "Open...",              "O", 0, 0, pCmdOpenFilesWindow },
    {   NM_ITEM,    "About...",              0 , 0, 0, pCmdAboutRequester },
    {   NM_ITEM,    NM_BARLABEL,             0 , 0, 0, 0 },
    {   NM_ITEM,    "Quit",                 "Q", 0, 0, pCmdQuit },
    { NM_END,     NULL,                      0 , 0, 0, 0 },
  };

  m_pMenu = CreateMenus(newMenu, TAG_DONE);
  if(m_pMenu == NULL)
  {
    throw "Failed to create the menu for FilesWindow.";
  }
}
