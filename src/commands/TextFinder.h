#ifndef TEXT_FINDER_H
#define TEXT_FINDER_H

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
class TextFinder : public CommandBase
{
public:
  TextFinder(std::vector<WindowBase*>* pAllWindowsVector,
            const DiffWorker& diffWorker,
            DiffWindow& diffWindow);
  
  virtual ~TextFinder();

  virtual void Execute(struct Window* pActiveWindow);

  const char* getSearchText() const;
  void setSearchText(const char* pSearchText);

  bool isCaseIgnored() const;
  void setCaseIgnored(bool isCaseIgnored);

  SearchLocation getLocation() const;
  void setLocation(SearchLocation location);

  SearchDirection getDirection() const;
  void setDirection(SearchDirection direction);

private:
  const DiffWorker& m_DiffWorker;
  DiffWindow& m_DiffWindow;

  SearchDirection m_Direction;

  const DiffDocument* m_pDiffDocument;
  DiffFileSearchEngine* m_pSearchEngine;
  DiffFileSearchEngine* m_pNewSearchEngine;

  DiffFileSearchEngine* createNewSearchEngine(const char* pSearchText, 
                                              bool isCaseIgnored, 
                                              SearchLocation location);

  bool hasDiffDocumentChanged() const;
};

#endif
