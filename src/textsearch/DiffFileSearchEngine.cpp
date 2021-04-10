#include <string.h>
#include "DiffFileSearchEngine.h"

DiffFileSearchEngine::DiffFileSearchEngine(const DiffFileBase& leftFile, const DiffFileBase& rightFile)
  : m_LeftFile(leftFile),
    m_RightFile(rightFile),
    m_pSearchResult(NULL)
{

}

DiffFileSearchEngine::~DiffFileSearchEngine()
{
  if(m_pSearchResult != NULL)
  {
    delete m_pSearchResult;
    m_pSearchResult = NULL;
  }
}

DiffFileSearchResult* DiffFileSearchEngine::findFirst(const char* pStrToSearch)
{
  for(size_t lineId = 0; lineId < m_LeftFile.getNumLines(); lineId++)
  {
    // Try to find pStrToSearch in left file
    const char* pFoundAt = strstr(m_LeftFile[lineId]->getText(), pStrToSearch);
    if(pFoundAt != NULL)
    {
      size_t charId = pFoundAt - m_LeftFile[lineId]->getText();

      if(m_pSearchResult != NULL)
      {
        delete m_pSearchResult;
      }

      m_pSearchResult = new DiffFileSearchResult(DiffFileSearchResult::LeftFile,
                                                 lineId,
                                                 charId);
      return m_pSearchResult;
    }

    pFoundAt = strstr(m_RightFile[lineId]->getText(), pStrToSearch);
    if(pFoundAt != NULL)
    {
      size_t charId = pFoundAt - m_RightFile[lineId]->getText();


      if(m_pSearchResult != NULL)
      {
        delete m_pSearchResult;
      }

      m_pSearchResult = new DiffFileSearchResult(DiffFileSearchResult::RightFile,
                                                 lineId,
                                                 charId);
      return m_pSearchResult;
    }
  }

  // Not found
  return NULL;
}

DiffFileSearchResult* DiffFileSearchEngine::findNext(const char* pStrToSearch)
{
  return NULL;
}
