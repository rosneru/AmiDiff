#ifndef CMD_SEARCH_H
#define CMD_SEARCH_H

#include <string>
#include "CommandBase.h"
#include "DiffDocument.h"
#include "DiffFileSearchEngine.h"
#include "DiffWindow.h"
#include "DiffWorker.h"

/**
 * Command to search for text in diff result window.
 *
 * @author Uwe Rosner
 * @date 12/04/2021
 */
class CmdSearch : public CommandBase
{
public:
  CmdSearch(std::vector<WindowBase*>* pAllWindowsVector,
            const DiffWorker& diffWorker,
            DiffWindow& diffWindow);
  
  virtual ~CmdSearch();

  virtual void Execute(struct Window* pActiveWindow);

  const char* getSearchText();
  void setSearchText(const char* pSearchText);

private:
  const DiffWorker& m_DiffWorker;
  DiffWindow& m_DiffWindow;
  std::string m_SearchText;
  const DiffDocument* m_pDiffDocument;
  DiffFileSearchEngine* m_pSearchEngine;
  long long m_LastFoundLineId;
};

#endif

