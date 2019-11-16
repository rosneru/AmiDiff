#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "Array.h"
#include "DiffLine.h"
#include "ProgressReporter.h"


/**
 * Class for storing a file line by line with diff information.
 *
 * It originates from Stephane Rodriguez open diff implementation:
 * https://www.codeproject.com/Articles/3666/Diff-tool
 *
 * @author Uwe Rosner
 * @date 28/09/2018
 */
class DiffFilePartition
{
  // Constructor
public:
  DiffFilePartition(bool& p_bCancelRequested);

  long NumLines() const;
  void NumChanges(int& p_Added, int& p_Changed, int& p_Deleted) const;

  DiffLine* GetLine(size_t p_Index) const;
  const char* GetLineText(size_t p_Index) const;
  unsigned long GetLineToken(size_t p_Index) const;
  DiffLine::LineState GetLineState(size_t p_Index) const;
  void SetLineState(size_t p_Index, DiffLine::LineState state);

  virtual void AddString(const char*, DiffLine::LineState) = 0;

  void AddBlankLine();

  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

protected:
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;
  Array<DiffLine*> m_DiffLinesArray;
};

#endif
