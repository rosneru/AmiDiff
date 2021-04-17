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

    // TODO This searches both files completely and could take some
    // time. Consider to create a task.
    m_pSearchEngine = new DiffFileSearchEngine(m_pDiffDocument->getLeftDiffFile(),
                                               m_pDiffDocument->getRightDiffFile(),
                                               m_SearchText.c_str());

  }

  DiffFileSearchResult* pResult = m_pSearchEngine->getNextResult();
  if(pResult == NULL)
  {
    return;
  }

  if(pResult->getLocation() == DiffFileSearchResult::LeftFile)
  {

  }
  else
  {
    
  }
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
