#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "Array.h"
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
//  DiffFilePartition(LinkedList* p_pLinesList);
  virtual ~DiffFilePartition();

  Array<long>& TokensList();

  long NumLines() const;
  void NumChanges(int& p_Added, int& p_Changed, int& p_Deleted);

  const SimpleString GetIndexedLineText(size_t p_Index);
  DiffLine::LineState GetIndexedLineState(size_t p_Index);

  DiffLine* GetIndexedDiffLine(size_t p_Index);
  DiffLine* GetFirstDiffLine();
  DiffLine* GetNextDiffLine();
  DiffLine* GetPreviousDiffLine();
  DiffLine* GetCurrentDiffLine();

  bool PreProcess();

  bool MatchLine(long i1, DiffFilePartition& p_OtherFile, long& i2);

  void AddString(const SimpleString& p_String, DiffLine::LineState p_LineState);
  void AddString(const SimpleString& p_String);
  void AddBlankLine();

private:
  size_t m_Index;
  Array<SimpleString*> m_InputLinesArray;
  Array<DiffLine*> m_DiffLinesArray;
  Array<long> m_TokensArray;

  void clearDiffLinesList();
};

#endif
