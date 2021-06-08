#ifdef __clang__
  #include <clib/intuition_protos.h>
#else
  #include <proto/intuition.h>
#endif

#include "DiffFileSearchResult.h"
#include "CmdSearch.h"


CmdSearch::CmdSearch(std::vector<WindowBase*>* pAllWindowsVector,
                     const DiffWorker& diffWorker,
                     DiffWindow& diffWindow)
  : CommandBase(pAllWindowsVector),
    m_DiffWorker(diffWorker),
    m_DiffWindow(diffWindow),
    m_IsCaseIgnored(true),
    m_Location(SL_BothFiles),
    m_Direction(SD_Downward),
    m_pDiffDocument(NULL),
    m_pCurrentSearchEngine(NULL),
    m_pNewSearchEngine(NULL)
{
}

CmdSearch::~CmdSearch()
{
  if(m_pCurrentSearchEngine != NULL)
  {
    delete m_pCurrentSearchEngine;
    m_pCurrentSearchEngine = NULL;
  }

  if(m_pNewSearchEngine != NULL)
  {
    delete m_pNewSearchEngine;
    m_pCurrentSearchEngine = NULL;
  }
}

void CmdSearch::Execute(struct Window* pActiveWindow)
{
  if(m_SearchText.length() < 1)
  {
    return;
  }

  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  DiffWindowTextArea* pRightTextArea = m_DiffWindow.getRightTextArea();
  if((pLeftTextArea == NULL) || (pRightTextArea == NULL))
  {
    return;
  }

  DiffFileSearchResult* pResult = NULL;
  if(m_pNewSearchEngine != NULL)
  {
    // Apply the new search engine (which performed a search with
    // changed settings) if there is one
    if(m_pCurrentSearchEngine != NULL)
    {
      // Clear of former ('current') search engine
      pLeftTextArea->clearSelection();
      pRightTextArea->clearSelection();

      // Re-render old line to remove former selection from document
      if(m_pCurrentSearchEngine->getCurrentResult() != NULL)
      {
        m_DiffWindow
          .renderDocuments(m_pCurrentSearchEngine->getCurrentResult()->getLineId());
      }

      delete m_pCurrentSearchEngine;
      m_pCurrentSearchEngine = m_pNewSearchEngine;
      m_pNewSearchEngine = NULL;
    }

    if(m_Direction == SD_Downward)
    {
      pResult = m_pCurrentSearchEngine->getNextResult(pLeftTextArea->getY());
    }
    else
    {
      pResult = m_pCurrentSearchEngine->getPrevResult(pLeftTextArea->getY());
    }
  }
  else
  {
    if(m_Direction == SD_Downward)
    {
      pResult = m_pCurrentSearchEngine->getNextResult();
    }
    else
    {
      pResult = m_pCurrentSearchEngine->getPrevResult();
    }
  }

  if(pResult == NULL)
  {
    DisplayBeep(m_DiffWindow.getScreen().IntuiScreen());
    return;
  }


  int stopCharId = pResult->getCharId() + m_SearchText.length() - 1;
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
                                                  m_SearchText.length(),
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

bool CmdSearch::performSearch()
{
  if(m_SearchText.length() < 1)
  {
    return false;
  }

  DiffWindowTextArea* pLeftTextArea = m_DiffWindow.getLeftTextArea();
  DiffWindowTextArea* pRightTextArea = m_DiffWindow.getRightTextArea();
  if((pLeftTextArea == NULL) || (pRightTextArea == NULL))
  {
    return false;
  }

  //
  // Must create or re-create the SearchEngine
  //

  // Get current document
  m_pDiffDocument = m_DiffWorker.getDiffDocument();
  if(m_pDiffDocument == NULL)
  {
    return false;
  }

  if(m_pNewSearchEngine != NULL)
  {
    delete m_pNewSearchEngine;
  }

  // This searches all occurrences of m_SearchText in one or both files
  // (dependent on SearchLocation setting) and can take some time. 
  // TODO: Consider to create a task.
  m_pNewSearchEngine = new DiffFileSearchEngine(m_pDiffDocument->getLeftDiffFile(),
                                                m_pDiffDocument->getRightDiffFile(),
                                                m_SearchText.c_str(),
                                                m_IsCaseIgnored,
                                                m_Location);

  return m_pNewSearchEngine->getNumResults() > 0;
}


const char* CmdSearch::getSearchText() const
{
  return m_SearchText.c_str();
}

void CmdSearch::setSearchText(const char* pSearchText)
{
  if(m_SearchText == pSearchText)
  {
    // Search text didn't change
    return;
  }

  m_SearchText = pSearchText;

  // Perform the search with the changed search text
  performSearch();
}


bool CmdSearch::isCaseIgnored() const
{
  return m_IsCaseIgnored;
}


void CmdSearch::setCaseIgnored(bool isCaseIgnored)
{
  if(m_IsCaseIgnored == isCaseIgnored)
  {
    // Case ignored option didn't change
    return;
  }

  m_IsCaseIgnored = isCaseIgnored;

  // Perform the search with the changed ase ignored option
  performSearch();
}


SearchLocation CmdSearch::getLocation() const
{
  return m_Location;
}

void CmdSearch::setLocation(SearchLocation location)
{
  if(m_Location == location)
  {
    // Location didn't change
    return;
  }

  m_Location = location;

  // Perform the search with the changed location option
  performSearch();
}

SearchDirection CmdSearch::getDirection() const
{
  return m_Direction;
}

void CmdSearch::setDirection(SearchDirection direction)
{
  // No need to perform the search because this option doesn't affect
  // the search results. It is applied after search in commad execute
  // method.
  m_Direction = direction;
}


bool CmdSearch::hasDiffDocumentChanged() const
{
  const DiffDocument* pWorkerDiffDoc = m_DiffWorker.getDiffDocument();
  return m_pDiffDocument != pWorkerDiffDoc;
}
