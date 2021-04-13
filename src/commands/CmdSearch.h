#ifndef CMD_SEARCH_H
#define CMD_SEARCH_H

#include <string>
#include "CommandBase.h"

/**
 * Command to search for text in diff result window.
 *
 * @author Uwe Rosner
 * @date 12/04/2021
 */
class CmdSearch : public CommandBase
{
public:
  CmdSearch(std::vector<WindowBase*>* pAllWindowsVector);
  
  virtual ~CmdSearch();

  virtual void Execute(struct Window* pActiveWindow);

  const char* getSearchText();
  void setSearchText(const char* pSearchText);

private:
  std::string m_SearchText;
};

#endif

