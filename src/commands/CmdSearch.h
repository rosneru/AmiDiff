#ifndef CMD_SEARCH_H
#define CMD_SEARCH_H

#include <string>
#include "CommandBase.h"
#include "DiffDocument.h"
#include "DiffFileSearchEngine.h"
#include "DiffWindow.h"
#include "DiffWorker.h"
#include "SearchOptions.h"



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

  const char* getSearchText() const;
  void setSearchText(const char* pSearchText);

  bool isCaseIgnored() const;
  void setCaseIgnored(bool isCaseIgnored);

  SearchLocation getLocation() const;
  void setLocation(SearchLocation location);

  SearchDirection getDirection() const;
  void setDirection(SearchDirection direction);

  StartSearchFrom getStartFrom() const;
  void setStartFrom(StartSearchFrom startFrom);

private:
  const DiffWorker& m_DiffWorker;
  DiffWindow& m_DiffWindow;
  std::string m_SearchText;
  bool m_IsCaseIgnored;
  SearchLocation m_Location;
  SearchDirection m_Direction;
  StartSearchFrom m_StartFrom;

  const DiffDocument* m_pDiffDocument;
  DiffFileSearchEngine* m_pCurrentSearchEngine;
  DiffFileSearchEngine* m_pNewSearchEngine;
  long long m_LastFoundLineId;

  bool performSearch();

  bool hasDiffDocumentChanged() const;
};

#endif
