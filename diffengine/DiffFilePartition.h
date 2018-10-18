#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "LinkedList.h"
#include "SimpleString.h"

#include "DiffLine.h"

/**
 * Class for storing a file line by line with diff information.
 *
 * This is a port of Stephane Rodriguez source code on CodeProject:
 * https://www.codeproject.com/Articles/3666/Diff-tool
 *
 * In 2005 he released an updated version:
 * http://www.arstdesign.com/articles/difftool.html
 *
 * @author Uwe Rosner
 * @date 28/09/2018
 */
class DiffFilePartition
{
  // Constructor
public:
  DiffFilePartition();
  DiffFilePartition(LinkedList* p_pLinesList);
  virtual ~DiffFilePartition();

  size_t NumberOfLines();
  DiffLine* GetIndexedDiffLine(size_t p_Index);
  SimpleString* GetIndexedRawLine(size_t p_Index);
  DiffLine::LineState GetIndexedLineState(size_t p_Index);
  LinkedList* TokensList();

  bool PreProcess();
  bool MatchLine(long i1, DiffFilePartition& p_OtherFile, long& i2);

  void AddString(SimpleString* p_pString, DiffLine::LineState p_LineState);
  void AddString(SimpleString* p_pString);
  void AddBlankLine();

private:
  LinkedList* m_pInputLinesList;
  LinkedList* m_pDiffLinesList;
  LinkedList* m_pTokensList;

  void clearDiffLinesList();
  void clearTokensList();
};

#endif
