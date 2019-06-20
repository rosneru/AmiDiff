#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "SimpleString.h"
#include "DiffLine.h"
#include "DiffFilePartition.h"
#include "ProgressReporter.h"

/**
 * Class for generating a diff of the contents of two files.
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
class DiffEngine
{

public:
  DiffEngine(bool& p_bCancelRequested);

  bool Diff(DiffFilePartition& p_File1Src,
            DiffFilePartition& p_File2Src,
            DiffFilePartition& p_File1Diff,
            DiffFilePartition& p_File2Diff);

  /**
   * Setting the progress reporter
   */
  void SetProgressReporter(ProgressReporter* p_pProgressReporter);

private:
  Array<long> V;
  bool& m_bCancelRequested;
  ProgressReporter* m_pProgressReporter;  ///> for progress reporting

};

#endif
