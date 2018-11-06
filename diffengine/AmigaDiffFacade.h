#ifndef AMIGA_DIFF_FACADE_H
#define AMIGA_DIFF_FACADE_H

#include "DiffFilePartition.h"
#include "SimpleString.h"
#include "DiffWindow.h"

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
  AmigaDiffFacade();

  void SetLeftFilePath(const char* p_pLeftFilePath);
  SimpleString LeftFilePath();

  void SetRightFilePath(const char* p_pRightFilePath);
  SimpleString RightFilePath();

  void SetLeftDiffWindow(DiffWindow* p_pLeftWindow);
  void SetRightDiffWindow(DiffWindow* p_pRightWindow);

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
  DiffFilePartition* LeftFileDiff();

  /**
   * Returns the diff partition for right file or NULL if no diff was
   * performed.
   */
  DiffFilePartition* RightFileDiff();

private:
  SimpleString m_LeftFilePath;
  SimpleString m_RightFilePath;
  DiffFilePartition* m_pLeftFileDiff;
  DiffFilePartition* m_pRightFileDiff;
  DiffWindow* m_pLeftWindow;
  DiffWindow* m_pRightWindow;
};

#endif // AMIGA_DIFF_FACADE_H
