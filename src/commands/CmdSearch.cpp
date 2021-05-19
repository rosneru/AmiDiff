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

  DiffFileSearchResult* pResult;
  if((m_pDiffDocument != m_DiffWorker.getDiffDocument()) || 
     (m_pSearchEngine == NULL))
  {
    //
    // Must create or re-create the SearchEngine
    //

    // Get current document
    m_pDiffDocument = m_DiffWorker.getDiffDocument();
    if(m_pDiffDocument == NULL)
    {
      return;
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
                                               m_SearchText.c_str());

    pResult = m_pSearchEngine->getFirstResult(m_DiffWindow.getLeftTextArea()->getY());
    if(pResult == NULL)
    {
      DisplayBeep(m_DiffWindow.getScreen().IntuiScreen());
      return;
    }

    // Now again get the first result (to be displayed)
    pResult = m_pSearchEngine->getFirstResult(m_DiffWindow.getLeftTextArea()->getY());
  }
  else
  {
    // Get the next result (to be displayed)
    pResult = m_pSearchEngine->getNextResult();
  }
  
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


const char* CmdSearch::getSearchText()
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

  // With new search text the old search engine is obsolete
  if(m_pSearchEngine != NULL)
  {
    delete m_pSearchEngine;
    m_pSearchEngine = NULL;
  }
}
