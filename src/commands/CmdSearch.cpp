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
    m_StartFrom(SF_CurrentPage),
    m_pDiffDocument(NULL),
    m_pSearchEngine(NULL),
    m_LastFoundLineId(-1)
{
}

CmdSearch::~CmdSearch()
{
  if(m_pSearchEngine != NULL)
  {
    delete m_pSearchEngine;
    m_pSearchEngine = NULL;
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

  DiffFileSearchResult* pResult = m_pSearchEngine->getNextResult();
  if(pResult == NULL)
  {
    DisplayBeep(m_DiffWindow.getScreen().IntuiScreen());
    return;
  }

  if(m_LastFoundLineId > -1)
  {
    // Clear selection
    pLeftTextArea->clearSelection();
    pRightTextArea->clearSelection();

    // Re-render old line to remove former selection from document
    m_DiffWindow.renderDocuments(m_LastFoundLineId);
  }

  // Apply the new line
  m_LastFoundLineId = pResult->getLineId();

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
  bool hasScrolled = m_DiffWindow.scrollToVisible(pResult->getCharId(),
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

  // Search keyword changed, so all old selections must be cleared
  pLeftTextArea->clearSelection();
  pRightTextArea->clearSelection();
  if(m_LastFoundLineId > -1)
  {
    // Re-render old line to remove former selection from document
    m_DiffWindow.renderDocuments(m_LastFoundLineId);
  }

  m_LastFoundLineId = -1;

  if(m_pSearchEngine != NULL)
  {
    delete m_pSearchEngine;
  }

  // This searches all occurrences of m_SearchText in both files
  // and could take some time. TODO: Consider to create a task.
  m_pSearchEngine = new DiffFileSearchEngine(m_pDiffDocument->getLeftDiffFile(),
                                             m_pDiffDocument->getRightDiffFile(),
                                             m_SearchText.c_str(),
                                             m_IsCaseIgnored,
                                             m_Location);

  return m_pSearchEngine->getNumResults() > 0;
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
  // No need to perform the search because this option is after search
  // in commad execution method
  m_Direction = direction;
}


StartSearchFrom CmdSearch::getStartFrom() const
{
  // No need to perform the search because this option is after search
  // in commad execution method
  return m_StartFrom;
}

void CmdSearch::setStartFrom(StartSearchFrom startFrom)
{
  m_StartFrom = startFrom;
}


bool CmdSearch::didDiffDocumentChange() const
{
  const DiffDocument* pWorkerDiffDoc = m_DiffWorker.getDiffDocument();
  return m_pDiffDocument != pWorkerDiffDoc;
}

bool CmdSearch::didSearchParamsChange() const
{
  if(m_pSearchEngine == NULL)
  {
    return false;
  }

  if(m_SearchText != m_pSearchEngine->getSearchString())
  {
    return true;
  }

  if(m_IsCaseIgnored != m_pSearchEngine->isCaseIgnored())
  {
    return true;
  }

  if(m_Location != m_pSearchEngine->getLocation())
  {
    return true;
  }

  return false;
}
