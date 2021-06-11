#ifdef __clang__
  #include <clib/intuition_protos.h>
#else
  #include <proto/intuition.h>
#endif

#include <string.h>

#include "DiffFileSearchResult.h"
#include "CmdSearch.h"


CmdSearch::CmdSearch(std::vector<WindowBase*>* pAllWindowsVector,
                     const DiffWorker& diffWorker,
                     DiffWindow& diffWindow)
  : CommandBase(pAllWindowsVector),
    m_DiffWorker(diffWorker),
    m_DiffWindow(diffWindow),
    m_Direction(SD_Downward),
    m_pDiffDocument(NULL),
    m_pSearchEngine(NULL),
    m_pNewSearchEngine(NULL)
{
}

CmdSearch::~CmdSearch()
{
  if(m_pSearchEngine != NULL)
  {
    delete m_pSearchEngine;
    m_pSearchEngine = NULL;
  }

  if(m_pNewSearchEngine != NULL)
  {
    delete m_pNewSearchEngine;
    m_pNewSearchEngine = NULL;
  }
}

void CmdSearch::Execute(struct Window* pActiveWindow)
{
  if((m_pSearchEngine == NULL) && (m_pNewSearchEngine == NULL))
  {
    return;
  }

  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  DiffWindowTextArea* pRightTextArea = m_DiffWindow.getRightTextArea();
  if((pLeftTextArea == NULL) || (pRightTextArea == NULL))
  {
    return;
  }

  if(hasDiffDocumentChanged())
  {
    // Meanwhile the user 'opened' another diff document
    if(m_pSearchEngine != NULL)
    {
      m_pNewSearchEngine = createNewSearchEngine(m_pSearchEngine->getSearchString().c_str(),
                                                 m_pSearchEngine->isCaseIgnored(),
                                                 m_pSearchEngine->getLocation());
    }
  }

  if(m_pSearchEngine != NULL)
  {
    // And there's also an old search engine: Clear the old one
    pLeftTextArea->clearSelection();
    pRightTextArea->clearSelection();

    // Re-render the line line with the former search result to
    // visually remove the selection
    if(m_pSearchEngine->getCurrentResult() != NULL)
    {
      m_DiffWindow.renderDocuments(
        m_pSearchEngine->getCurrentResult()->getLineId());
    }
  }


  DiffFileSearchResult* pResult = NULL;
  if(m_pNewSearchEngine != NULL)
  {
    // Old search engine is not needed anymore as the new one will be
    // taken
    delete m_pSearchEngine;

    // The new search engine now becomes the current one
    m_pSearchEngine = m_pNewSearchEngine;

    // And is not 'the new search engine' anymore
    m_pNewSearchEngine = NULL;

    // Get the first search result in upward/downward direction from
    // current line
    if(m_Direction == SD_Downward)
    {
      pResult = m_pSearchEngine->getNextResult(pLeftTextArea->getY());
    }
    else
    {
      pResult = m_pSearchEngine->getPrevResult(pLeftTextArea->getY());
    }
  }
  else if(m_pSearchEngine != NULL)
  {
    // Get the prev/next search result
    if(m_Direction == SD_Downward)
    {
      pResult = m_pSearchEngine->getNextResult();
    }
    else
    {
      pResult = m_pSearchEngine->getPrevResult();
    }
  }

  if(pResult == NULL)
  {
    DisplayBeep(m_DiffWindow.getScreen().IntuiScreen());
    return;
  }

  size_t searchStringLength = m_pSearchEngine->getSearchString().length();
  int stopCharId = pResult->getCharId() + searchStringLength - 1;

  if(pResult->getLocation() == DiffFileSearchResult::LeftFile)
  {
    pLeftTextArea->addSelection(pResult->getLineId(),
                                pResult->getCharId(),
                                stopCharId);
  }
  else if(pResult->getLocation() == DiffFileSearchResult::RightFile)
  {
    pRightTextArea->addSelection(pResult->getLineId(),
                                 pResult->getCharId(),
                                 stopCharId);
  }

  // If necessary scroll the window to have the result visible
  bool hasScrolled = m_DiffWindow.scrollToPage(pResult->getCharId(),
                                               pResult->getLineId(),
                                               searchStringLength,
                                               1);

  if(hasScrolled)
  {
    // Re-render complete document
    m_DiffWindow.renderDocuments();
  }
  else
  {
    // Re-render only line with the result
    m_DiffWindow.renderDocuments(pResult->getLineId());
  }
}

DiffFileSearchEngine* CmdSearch::createNewSearchEngine(const char* pSearchText,
                                                       bool isCaseIgnored,
                                                       SearchLocation location)
{
  DiffFileSearchEngine* pNewSearchEngine = NULL;

  if((pSearchText == NULL) || (strlen(pSearchText) < 1))
  {
    return NULL;
  }

  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  DiffWindowTextArea* pRightTextArea = m_DiffWindow.getRightTextArea();
  if((pLeftTextArea == NULL) || (pRightTextArea == NULL))
  {
    return NULL;
  }


  // Get current document
  m_pDiffDocument = m_DiffWorker.getDiffDocument();
  if(m_pDiffDocument == NULL)
  {
    return NULL;
  }

  // This already performes the search of all occurrences of pSearchText
  // in one or both files (dependent on location parameter) and can take
  // some time.
  //
  // TODO: Consider to create a task.
  pNewSearchEngine = new DiffFileSearchEngine(m_pDiffDocument->getLeftDiffFile(),
                                              m_pDiffDocument->getRightDiffFile(),
                                              pSearchText,
                                              isCaseIgnored,
                                              location);

  return pNewSearchEngine;
}


const char* CmdSearch::getSearchText() const
{
  if(m_pSearchEngine == NULL)
  {
    return ""; // Default
  }

  return m_pSearchEngine->getSearchString().c_str();
}

void CmdSearch::setSearchText(const char* pSearchText)
{
  if(m_pNewSearchEngine != NULL)
  {
    if(m_pNewSearchEngine->getSearchString() == pSearchText)
    {
      // Search text hasn't changed
      return;
    }

    // Apply changed search text. But there's already a new created
    // search engine with some other search parameter changed. So we
    // create another new search engine and take the other parameters
    // from the already existing new search engine.
    DiffFileSearchEngine* pTmpSearchEngine = NULL;
    pTmpSearchEngine = createNewSearchEngine(pSearchText,
                                             m_pNewSearchEngine->isCaseIgnored(),
                                             m_pNewSearchEngine->getLocation());

    delete m_pNewSearchEngine;
    m_pNewSearchEngine = pTmpSearchEngine;
  }
  else if(m_pSearchEngine != NULL)
  {
    if(m_pSearchEngine->getSearchString() == pSearchText)
    {
      // Search text hasn't changed
      return;
    }

    // Apply changed search text. A new search engine is created which
    // takes all other parameters from the current search engine. from
    // the already existing new search engine.
    m_pNewSearchEngine = createNewSearchEngine(pSearchText,
                                               m_pSearchEngine->isCaseIgnored(),
                                               m_pSearchEngine->getLocation());
  }
  else
  {
    // No search engine exists
    m_pNewSearchEngine = createNewSearchEngine(pSearchText,
                                               isCaseIgnored(),
                                               getLocation());
  }
}


bool CmdSearch::isCaseIgnored() const
{
  if(m_pSearchEngine == NULL)
  {
    return true;  // Default
  }

  return m_pSearchEngine->isCaseIgnored();
}


void CmdSearch::setCaseIgnored(bool isCaseIgnored)
{
  if(m_pNewSearchEngine != NULL)
  {
    if(m_pNewSearchEngine->isCaseIgnored() == isCaseIgnored)
    {
      // Case ignored flag hasn't changed
      return;
    }

    // Apply changed isCaseIgnored parameter. But there's already a new
    // created search engine with some other search parameter changed.
    // So we create another new search engine and take the other
    // parameters from the already existing new search engine.
    DiffFileSearchEngine* pTmpSearchEngine = NULL;
    pTmpSearchEngine = createNewSearchEngine(m_pNewSearchEngine->getSearchString().c_str(),
                                             isCaseIgnored,
                                             m_pNewSearchEngine->getLocation());

    delete m_pNewSearchEngine;
    m_pNewSearchEngine = pTmpSearchEngine;
  }
  else if(m_pSearchEngine != NULL)
  {
    if(m_pSearchEngine->isCaseIgnored() == isCaseIgnored)
    {
      // Case ignored flag hasn't changed
      return;
    }

    // Apply changed isCaseIgnored parameter. A new search engine is
    // created which takes all other parameters from the current search
    // engine. from the already existing new search engine.
    m_pNewSearchEngine = createNewSearchEngine(m_pSearchEngine->getSearchString().c_str(),
                                               isCaseIgnored,
                                               m_pSearchEngine->getLocation());
  }
  else
  {
    // No search engine exists
    m_pNewSearchEngine = createNewSearchEngine(getSearchText(),
                                               isCaseIgnored,
                                               getLocation());
  }
}


SearchLocation CmdSearch::getLocation() const
{
  if(m_pSearchEngine == NULL)
  {
    return SL_BothFiles;  // Default
  }

  return m_pSearchEngine->getLocation();
}

void CmdSearch::setLocation(SearchLocation location)
{
  if(m_pNewSearchEngine != NULL)
  {
    if(m_pNewSearchEngine->getLocation() == location)
    {
      // Location hasn't changed
      return;
    }

    // Apply changed location parameter. But there's already a new
    // created search engine with some other search parameter changed.
    // So we create another new search engine and take the other
    // parameters from the already existing new search engine.
    DiffFileSearchEngine* pTmpSearchEngine = NULL;
    pTmpSearchEngine = createNewSearchEngine(m_pNewSearchEngine->getSearchString().c_str(),
                                             m_pNewSearchEngine->isCaseIgnored(),
                                             location);

    delete m_pNewSearchEngine;
    m_pNewSearchEngine = pTmpSearchEngine;
  }
  else if(m_pSearchEngine != NULL)
  {
    if(m_pSearchEngine->getLocation() == location)
    {
      // Location hasn't changed
      return;
    }

    // Apply changed location parameter. A new search engine is created
    // which takes all other parameters from the current search engine.
    // from the already existing new search engine.
    m_pNewSearchEngine = createNewSearchEngine(m_pSearchEngine->getSearchString().c_str(),
                                               m_pSearchEngine->isCaseIgnored(),
                                               location);
  }
  else
  {
    // No search engine exists
    m_pNewSearchEngine = createNewSearchEngine(getSearchText(),
                                               isCaseIgnored(),
                                               location);
  }
}

SearchDirection CmdSearch::getDirection() const
{
  return m_Direction;
}

void CmdSearch::setDirection(SearchDirection direction)
{
  // No need to perform the search / create a new search engine because
  // this option doesn't affect the search results. It is applied
  // directly in commad execute method.
  m_Direction = direction;
}


bool CmdSearch::hasDiffDocumentChanged() const
{
  const DiffDocument* pWorkerDiffDoc = m_DiffWorker.getDiffDocument();
  return m_pDiffDocument != pWorkerDiffDoc;
}
