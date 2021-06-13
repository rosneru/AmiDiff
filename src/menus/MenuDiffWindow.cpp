#ifdef __clang__
  #include <clib/gadtools_protos.h>
#else
  #include <proto/gadtools.h>
#endif

#include "MenuDiffWindow.h"

MenuDiffWindow::MenuDiffWindow(CommandBase* pCmdOpenFilesWindow,
                               CommandBase* pCmdAboutRequester,
                               CommandBase* pCmdQuit,
                               CommandBase* pCmdNavPrevDiff,
                               CommandBase* pCmdNavNextDiff,
                               CommandBase* pCmdOpenSearchWindow,
                               CommandBase* pCmdFindNext,
                               CommandBase* pCmdFindPrev)
{
  struct NewMenu newMenu[] = 
  {
    { NM_TITLE,   "Project",                 0 , 0, 0, 0 },
    {   NM_ITEM,    "Open...",              "O", 0, 0, pCmdOpenFilesWindow },
    {   NM_ITEM,    "About...",              0 , 0, 0, pCmdAboutRequester },
    {   NM_ITEM,    NM_BARLABEL,             0 , 0, 0, 0 },
    {   NM_ITEM,    "Quit",                 "Q", 0, 0, pCmdQuit },
    { NM_TITLE,   "Navigate",                0 , 0, 0, 0 },
    {   NM_ITEM,    "Previous difference",  "P", 0, 0, pCmdNavPrevDiff },
    {   NM_ITEM,    "Next difference",      "N", 0, 0, pCmdNavNextDiff },
    {   NM_ITEM,    NM_BARLABEL,             0 , 0, 0, 0 },
    {   NM_ITEM,    "Find...",              "F", 0, 0, pCmdOpenSearchWindow },
    {   NM_ITEM,    "Find prev",            "2", 0, 0, pCmdFindPrev },
    {   NM_ITEM,    "Find next",            "3", 0, 0, pCmdFindNext },
    { NM_END,     NULL,                      0 , 0, 0, 0 },
  };

  m_pMenu = CreateMenus(newMenu, TAG_DONE);
  if(m_pMenu == NULL)
  {
    throw "Failed to create the menu for DiffWindow.";
  }
}
