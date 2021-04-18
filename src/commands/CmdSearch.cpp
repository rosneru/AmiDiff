#include "DiffFileSearchResult.h"
#include "CmdSearch.h"


CmdSearch::CmdSearch(std::vector<WindowBase*>* pAllWindowsVector,
                     const DiffWorker& diffWorker,
                     DiffWindow& diffWindow)
  : CommandBase(pAllWindowsVector),
    m_DiffWorker(diffWorker),
    m_DiffWindow(diffWindow),
    m_pDiffDocument(NULL),
    m_pSearchEngine(NULL)
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
    // Must create or re-create the DiffEngine
    if(m_pSearchEngine == NULL)
    {
      delete m_pSearchEngine;
    }

    m_pDiffDocument = m_DiffWorker.getDiffDocument();
    if(m_pDiffDocument == NULL)
    {
      return;
    }

    // This searches all occurences of m_SearchText in both files
    // and could take some time. TODO: Consider to create a task.
    m_pSearchEngine = new DiffFileSearchEngine(m_pDiffDocument->getLeftDiffFile(),
                                               m_pDiffDocument->getRightDiffFile(),
                                               m_SearchText.c_str());

    pResult = m_pSearchEngine->getFirstResult();
    if(pResult == NULL)
    {
      return;
    }

    // Search keyword changed, so all old selections muts be cleared
    pLeftTextArea->clearSelection();
    pRightTextArea->clearSelection();

    int lenDelta = m_SearchText.length() - 1;

    do
    {
      if(pResult->getLocation() == DiffFileSearchResult::LeftFile)
      {
        pLeftTextArea->addSelection(pResult->getLineId(), 
                                    pResult->getCharId(),
                                    pResult->getCharId() + lenDelta);
      }
      else if(pResult->getLocation() == DiffFileSearchResult::RightFile)
      {
        pRightTextArea->addSelection(pResult->getLineId(), 
                                     pResult->getCharId(),
                                     pResult->getCharId() + lenDelta);
      }
    } while ((pResult = m_pSearchEngine->getNextResult()) != NULL);
    
    // Now again get the first result (to be displayed)
    pResult = m_pSearchEngine->getFirstResult();
  }
  else
  {
    // Get the next result (to be displayed)
    pResult = m_pSearchEngine->getNextResult();
  }
  
  if(pResult == NULL)
  {
    return;
  }

  // Now scroll the text area in question to the result position
  m_DiffWindow.handleXChange(pResult->getCharId());
  m_DiffWindow.handleYChange(pResult->getLineId());
}


const char* CmdSearch::getSearchText()
{
  return m_SearchText.c_str();
}

void CmdSearch::setSearchText(const char* pSearchText)
{
  if(m_SearchText.c_str() == pSearchText)
  {
    // Search text didn't change
    return;
  }

  m_SearchText = pSearchText;

  // With new search text the old search engione is obsolete
  if(m_pSearchEngine != NULL)
  {
    delete m_pSearchEngine;
    m_pSearchEngine = NULL;
  }
}
