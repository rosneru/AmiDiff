#ifndef DIFF_FILE_SEARCH_ENGINE_H
#define DIFF_FILE_SEARCH_ENGINE_H

#include <string>
#include <vector>

#include "DiffFileBase.h"
#include "DiffFileSearchResult.h"
#include "SearchConstants.h"

/**
 * A text search engine that operates on two DiffFiles.
 *
 * @author Uwe Rosner
 * @date 10/04/2021
 */
class DiffFileSearchEngine
{
public:
  DiffFileSearchEngine(const DiffFileBase& leftFile, 
                       const DiffFileBase& rightFile, 
                       const char* pSearchString,
                       bool isCaseIgnored,
                       SearchLocation location);
  virtual ~DiffFileSearchEngine();

  size_t getNumResults();
  DiffFileSearchResult* getFirstResult(size_t startLineId);
  DiffFileSearchResult* getPrevResult();
  DiffFileSearchResult* getNextResult();

  const std::string& getSearchString() const;
  bool isCaseIgnored() const;
  SearchLocation getLocation() const;

private:
  const DiffFileBase& m_LeftFile;
  const DiffFileBase& m_RightFile;
  std::string m_SearchString;
  bool m_IsCaseIgnored;
  SearchLocation m_Location;

  std::vector<DiffFileSearchResult*> m_Results;
  std::vector<DiffFileSearchResult*>::iterator m_ResultsIterator;

  void find();
  

};

#endif
