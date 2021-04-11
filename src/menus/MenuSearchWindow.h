#ifndef MENU_SEARCH_WINDOW_H
#define MENU_SEARCH_WINDOW_H

#include "CommandBase.h"
#include "MenuBase.h"

class MenuSearchWindow : public MenuBase
{
public:
  MenuSearchWindow(CommandBase* pCmdOpenFilesWindow,
                   CommandBase* pCmdAboutRequester,
                   CommandBase* pCmdQuit,
                   CommandBase* pCmdFind,
                   CommandBase* pCmdFindNext);
};

#endif
