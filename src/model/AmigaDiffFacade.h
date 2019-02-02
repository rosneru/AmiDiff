#ifndef AMIGA_DIFF_FACADE_H
#define AMIGA_DIFF_FACADE_H

#include <exec/ports.h>

#include "BackgroundWorker.h"
#include "DiffDocument.h"
#include "DiffFilePartition.h"
#include "DiffFilePartitionAmiga.h"
#include "DiffWindow.h"
#include "FilesWindow.h"
#include "ProgressWindow.h"
#include "SimpleString.h"
#include "StopWatch.h"

/**
 * Facade to hold all objects which are needed to perform a diff and
 * display its results
 *
 *
 * @author Uwe Rosner
 * @date 26/10/2018
 */
class AmigaDiffFacade : public BackgroundWorker
{
public:
  AmigaDiffFacade(SimpleString& p_LeftFilePath,
                  SimpleString& p_RightFilePath,
                  DiffWindow& p_DiffWindow,
                  FilesWindow& p_FilesWindow,
                  ProgressWindow& p_ProgressWindow,
                  struct MsgPort* p_pProgressPort);

  virtual ~AmigaDiffFacade();

  const SimpleString& ErrorText() const;
  const SimpleString& ElapsedText() const;


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
  DiffWindow& m_DiffWindow;
  FilesWindow& m_FilesWindow;
  ProgressWindow& m_ProgressWindow;
  int m_ProgressOffset;

  SimpleString& m_LeftFilePath;
  SimpleString& m_RightFilePath;

  DiffFilePartitionAmiga m_LeftSrcPartition;
  DiffFilePartitionAmiga m_RightSrcPartition;

  DiffFilePartition m_LeftDiffPartition;
  DiffFilePartition m_RightDiffPartition;

  DiffDocument* m_pLeftDiffDocument;
  DiffDocument* m_pRightDiffDocument;

  SimpleString m_ErrorText;   ///> Information about last error

  StopWatch m_StopWatch;
  SimpleString m_ElapsedText; ///> Information about elapsed times

  void disposeDocuments();

  void doWork();

  /**
   * Sending a message about progress to the port given in the
   * constructor.
   *
   * NOTE Extends the basis class method. The 3 stages of progress in
   *      in diff are reported alltogether as 0..33%, 33%..66% and
   *      66%..100%.
   */
  virtual void notifyProgressChanged(int p_Progress);

};

#endif // AMIGA_DIFF_FACADE_H
