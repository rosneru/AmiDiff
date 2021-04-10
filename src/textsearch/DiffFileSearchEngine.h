#ifndef DIFF_FILE_SEARCH_ENGINE_H
#define DIFF_FILE_SEARCH_ENGINE_H

#include "DiffFileBase.h"
#include "DiffFileSearchResult.h"

/**
 * A text search engine that operates on two DiffFiles.
 *
 * @author Uwe Rosner
 * @date 10/04/2021
 */
class DiffFileSearchEngine
{
public:
  DiffFileSearchEngine(const DiffFileBase& leftFile, const DiffFileBase& rightFile);
  virtual ~DiffFileSearchEngine();

  DiffFileSearchResult* findFirst(const char* pStrToSearch);
  DiffFileSearchResult* findNext(const char* pStrToSearch);

private:
  const DiffFileBase& m_LeftFile;
  const DiffFileBase& m_RightFile;
  DiffFileSearchResult* m_pSearchResult;
  const char* m_pStrToSearch;

};

#endif
