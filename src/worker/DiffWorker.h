#ifndef DIFF_WORKER_H
#define DIFF_WORKER_H

#include <exec/ports.h>

#include "CmdCloseWindow.h"
#include "CmdOpenWindow.h"
#include "DiffDocument.h"
#include "DiffEngineAmiga.h"
#include "DiffFileAmiga.h"
#include "DiffWindow.h"
#include "FilesWindow.h"
#include "LinkedListAmiga.h"
#include "ProgressWindow.h"
#include "SimpleString.h"
#include "StopWatch.h"
#include "WorkerBase.h"

/**
 * Class to perform the diff as a background process. Also managing
 * the opening and closing of the required windows to e.g. display the
 * file differences.
 *
 *
 * @author Uwe Rosner
 * @date 26/10/2018
 */
class DiffWorker : public WorkerBase
{
public:
  DiffWorker(SimpleString& leftFilePath,
             SimpleString& rightFilePath,
             DiffWindow& diffWindow,
             ProgressWindow& progressWindow,
             CmdOpenWindow& cmdOpenFilesWindow,
             CmdCloseWindow& cmdCloseFilesWindow,
             struct MsgPort*& pProgressPort,
             bool& bCancelRequested,
             bool& bExitAllowed);

  virtual ~DiffWorker();

  /**
   * When called before performing a diff, the line numbers for the 
   * files are stored and displayed in the DiffWindow depending on
   * the given value.
   */
  void SetLineNumbers(bool bEnabled);

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
  DiffFileBase& LeftFileDiff();

  /**
   * Returns the diff partition for right file or NULL if no diff was
   * performed.
   */
  DiffFileBase& RightFileDiff();

private:
  SimpleString& m_LeftSrcFilePath;
  SimpleString& m_RightSrcFilePath;

  DiffWindow& m_DiffWindow;
  ProgressWindow& m_ProgressWindow;
  CmdOpenWindow& m_CmdOpenFilesWindow;
  CmdCloseWindow& m_CmdCloseFilesWindow;
  int m_ProgressOffset;
  bool& m_bCancelRequested;
  bool& m_bExitAllowed;

  APTR m_pPoolHeader;

  LinkedListAmiga m_DiffStartIdxsList;

  DiffDocument* m_pDiffDocument;

  DiffFileAmiga m_LeftSrcFile;
  DiffFileAmiga m_RightSrcFile;
  DiffFileAmiga m_LeftDiffFile;
  DiffFileAmiga m_RightDiffFile;

  DiffEngineAmiga m_DiffEngine;

  bool m_bShowLineNumbers;
  
  StopWatch m_StopWatch;


  void disposeDocuments();

  /**
   * Implemented abstract method from WorkerBase.
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
