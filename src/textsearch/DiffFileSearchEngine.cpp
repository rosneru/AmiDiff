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

}

DiffFileSearchResult* DiffFileSearchEngine::findNext(const char* pStrToSearch)
{

}
