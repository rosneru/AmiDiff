#include <string.h>
#include "DiffFileSearchEngine.h"

DiffFileSearchEngine::DiffFileSearchEngine(const DiffFileBase& leftFile, 
                       const DiffFileBase& rightFile, 
                       const char* pSearchString)
  : m_LeftFile(leftFile),
    m_RightFile(rightFile),
    m_SearchString(pSearchString)
{
  search();
}

DiffFileSearchEngine::~DiffFileSearchEngine()
{
  std::vector<DiffFileSearchResult*>::iterator it;
  for(it = m_Results.begin(); it != m_Results.end(); it++)
  {
    delete *it;
  }
}

DiffFileSearchResult* DiffFileSearchEngine::getFirstResult()
{
  if(m_Results.size() < 1)
  {
    // Search string not found
    return NULL;
  }

  std::vector<DiffFileSearchResult*>::iterator it = m_Results.begin();
  return *it;
}

DiffFileSearchResult* DiffFileSearchEngine::getNextResult()
{
  return NULL;
}



void DiffFileSearchEngine::search()
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