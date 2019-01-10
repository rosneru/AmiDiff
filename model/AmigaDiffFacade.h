#ifndef AMIGA_DIFF_FACADE_H
#define AMIGA_DIFF_FACADE_H

#include "DiffDocument.h"
#include "DiffFilePartition.h"
#include "DiffFilePartitionAmiga.h"
#include "DiffWindow.h"
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
class AmigaDiffFacade
{
public:
  AmigaDiffFacade(DiffWindow& p_DiffWindow);
  ~AmigaDiffFacade();

  void SetLeftFilePath(const SimpleString& p_LeftFilePath);
  const SimpleString& LeftFilePath() const;

  void SetRightFilePath(const SimpleString& p_RightFilePath);
  const SimpleString& RightFilePath() const;

  const SimpleString& ErrorText() const;

  const SimpleString& ElapsedText() const;


  /**
   * Performs the diff using LeftFilePath and RightFilePath as input
   * files. After the the diff is sucessfully created, the resulting
   * diff can be retrievced using LeftFileDiff and RightFileDiff in
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

  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;

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
};

#endif // AMIGA_DIFF_FACADE_H
