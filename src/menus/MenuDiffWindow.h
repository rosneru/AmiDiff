#ifndef MENU_DIFF_WINDOW_H
#define MENU_DIFF_WINDOW_H

#include "CommandBase.h"
#include "MenuBase.h"

class MenuDiffWindow : public MenuBase
{
public:
  MenuDiffWindow(CommandBase* pCmdOpenFilesWindow,
                 CommandBase* pCmdAboutRequester,
                 CommandBase* pCmdQuit,
                 CommandBase* pCmdNavigateFirstDiff,
                 CommandBase* pCmdNavigatePrevDiff,
                 CommandBase* pCmdNavigateNextDiff,
                 CommandBase* pCmdNavigateLastDiff,
                 CommandBase* pCmdOpenSearchWindow,
                 CommandBase* pCmdFindNext,
                 CommandBase* pCmdFindPrev);
};

#endif
