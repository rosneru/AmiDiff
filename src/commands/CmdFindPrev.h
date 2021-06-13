#ifndef CMD_FIND_PREV_H
#define CMD_FIND_PREV_H

#include "TextFinder.h"
#include "CommandBase.h"

/**
 * Command to display the next search result
 *
 * @author Uwe Rosner
 * @date 13/06/2021
 */
class CmdFindPrev : public CommandBase
{
public:
  CmdFindPrev(std::vector<WindowBase*>* pAllWindowsVector,
              TextFinder& textFinder);

  virtual ~CmdFindPrev();
  virtual void Execute(struct Window* pActiveWindow);

private:
  TextFinder& m_TextFinder;
};

#endif
