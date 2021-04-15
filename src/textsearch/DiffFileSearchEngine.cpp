#include <string.h>
#include "DiffFileSearchEngine.h"

DiffFileSearchEngine::DiffFileSearchEngine(const DiffFileBase& leftFile, 
                       const DiffFileBase& rightFile, 
                       const char* pSearchString)
  : m_LeftFile(leftFile),
    m_RightFile(rightFile),
    m_SearchString(pSearchString),
    m_ResultsIterator(m_Results.end())
{
  find();
  m_ResultsIterator = m_Results.end();
}

DiffFileSearchEngine::~DiffFileSearchEngine()
{
  std::vector<DiffFileSearchResult*>::iterator it;
  for(it = m_Results.begin(); it != m_Results.end(); it++)
  {
    delete *it;
  }
}


size_t DiffFileSearchEngine::getNumResults()
{
  return m_Results.size();
}

DiffFileSearchResult* DiffFileSearchEngine::getPrevResult()
{
  if(m_ResultsIterator == m_Results.end())
  {
    // Iterator points to the end. This only is true directly after the
    // creation of the SearchEngine. Set it to the first item.
    m_ResultsIterator = m_Results.begin();
  }
  else if(m_ResultsIterator != m_Results.begin())
  {
    // Only if not already the first item
    m_ResultsIterator--;
  }

  return (*m_ResultsIterator);
}

DiffFileSearchResult* DiffFileSearchEngine::getNextResult()
{
  if(m_ResultsIterator == m_Results.end())
  {
    // Iterator points to the end. This only is true directly after the
    // creation of the SearchEngine. Set it to the first item.
    m_ResultsIterator = m_Results.begin();
  }
  else
  {
    m_ResultsIterator++;

    if(m_ResultsIterator == m_Results.end())
    {
      // Avoid overflow: back to last valid item
      m_ResultsIterator--;
    }
  }

    return (*m_ResultsIterator);
}



void DiffFileSearchEngine::find()
{
  DiffFileSearchResult* pResult;
  for(size_t lineId = 0; lineId < m_LeftFile.getNumLines(); lineId++)
  {
    const char* pSearchStart = m_LeftFile[lineId]->getText();

    // Try to find pStrToSearch in left file
    const char* pFoundAt = strstr(pSearchStart, m_SearchString.c_str());
    while (pFoundAt != NULL)
    {
      size_t charId = pFoundAt - m_LeftFile[lineId]->getText();

      pResult = new DiffFileSearchResult(DiffFileSearchResult::LeftFile,
                                         lineId,
                                         charId);
      m_Results.push_back(pResult);

      pFoundAt = strstr(pFoundAt + 1, m_SearchString.c_str());
    } 
  }
}