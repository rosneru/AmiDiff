#ifndef DIFF_WORKER_H
#define DIFF_WORKER_H

#include <vector>

#include <exec/ports.h>

#include "CmdCloseWindow.h"
#include "CmdOpenWindow.h"
#include "DiffDocument.h"
#include "DiffOutputFileAmiga.h"
#include "DiffInputFileAmiga.h"
#include "DiffWindow.h"
#include "DiffWorkerProgressReporter.h"
#include "FilesWindow.h"
#include "ProgressWindow.h"
#include "SimpleString.h"
#include "StopWatch.h"
#include "WorkerBase.h"

/**
 * Class to perform the diff as a background process. Also managing
 * the opening and closing of the required windows to e.g. display the
 * file differences.

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
  DiffWorkerProgressReporter m_Progress;
  StopWatch m_StopWatch;
  DiffWindow& m_DiffWindow;
  ProgressWindow& m_ProgressWindow;
  CmdOpenWindow& m_CmdOpenFilesWindow;
  CmdCloseWindow& m_CmdCloseFilesWindow;
  bool& m_bCancelRequested;
  bool& m_bExitAllowed;

  DiffDocument* m_pDiffDocument;


  bool m_bShowLineNumbers;


  /**
   * Implemented abstract method from WorkerBase.
   *
   * Here the diff is calculated, the display window opened etc.
   */
  void doWork();

};

#endif
