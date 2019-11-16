#ifndef DIFF_WORKER_H
#define DIFF_WORKER_H

#include <exec/ports.h>

#include "BackgroundWorker.h"
#include "DiffDocument.h"
#include "DiffEngine.h"
#include "DiffFilePartition.h"
#include "DiffFilePartitionAmiga.h"
#include "DiffWindow.h"
#include "FilesWindow.h"
#include "ProgressWindow.h"
#include "SimpleString.h"
#include "StopWatch.h"

/**
 * Class to perform the diff as a background process. Also managing
 * the opening and closing of the required windows to e.g. display the
 * file differences.
 *
 *
 * @author Uwe Rosner
 * @date 26/10/2018
 */
class DiffWorker : public BackgroundWorker
{
public:
  DiffWorker(SimpleString& leftFilePath,
             SimpleString& rightFilePath,
             DiffWindow& diffWindow,
             FilesWindow& filesWindow,
             ProgressWindow& progressWindow,
             struct MsgPort* pProgressPort,
             bool& bCancelRequested,
             bool& bExitAllowed);

  virtual ~DiffWorker();

  /**
   * Performs the diff using LeftFilePath and RightFilePath as input
   * files. After the the diff is successfully created, the resulting
   * diff can be retrieved using LeftFileDiff and RightFileDiff in
   * the host application.
   *
   * @returns
   * When successful: true; false else.
   */
  bool Diff();

  /**
   * Returns the diff partition for left file or NULL if no diff was
   * performed.
   */
  DiffFilePartition& LeftFileDiff();

  /**
   * Returns the diff partition for right file or NULL if no diff was
   * performed.
   */
  DiffFilePartition& RightFileDiff();

private:
  SimpleString& m_LeftFilePath;
  SimpleString& m_RightFilePath;

  DiffWindow& m_DiffWindow;
  FilesWindow& m_FilesWindow;
  ProgressWindow& m_ProgressWindow;
  int m_ProgressOffset;
  bool& m_bCancelRequested;
  bool& m_bExitAllowed;

  DiffDocument* m_pLeftDiffDocument;
  DiffDocument* m_pRightDiffDocument;

  DiffFilePartitionAmiga m_LeftSrcPartition;
  DiffFilePartitionAmiga m_RightSrcPartition;

  DiffFilePartitionAmiga m_LeftDiffPartition;
  DiffFilePartitionAmiga m_RightDiffPartition;

  DiffEngine m_DiffEngine;

  StopWatch m_StopWatch;

  void disposeDocuments();

  /**
   * Implemented abstract method from BackgroundWorker.
   *
   * Here the diff is calculated, the display window opened etc.
   */
  void doWork();

  /**
   * Sending a message about progress to the port given in the
   * constructor.
   *
   * NOTE Extends the basis class method. The 3 stages of progress in
   *      diff (pre-processing left file, pre-processing right file
   *      and performing the diff) are reported merged with 0..33%,
   *      33%..66% and 66%..100%.
   */
  virtual void notifyProgressChanged(int progress);

};

#endif