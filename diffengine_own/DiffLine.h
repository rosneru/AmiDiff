/* DiffLine.h                                               */
/*-------------------------------------------------------------------*/
/* Class for storing a file line by line with diff information       */
/*                                                                   */
/* This is an adaption of Stephane Rodriguez source code on          */
/* CodeProject: https://www.codeproject.com/Articles/3666/Diff-tool  */
/*                                                                   */
/* Uwe Rosner                                                        */
/* 09/2018                                                           */
/*-------------------------------------------------------------------*/

#ifndef DIFF_LINE_H
#define DIFF_LINE_H

#include "LinkedList.h"
#include "SimpleString.h"


class DiffLine
{
public:
  enum LineStatus
  {
    Normal=0,
    Changed,
    Added,
    Deleted
  };

  DiffLine();
  ~DiffLine();

  long SetLine(SimpleString* p_pString); // store string and build token
  void SetLine(SimpleString* p_pString, LineStatus p_LineStatus); // store string and status (does not eval token)
  SimpleString* GetLine();

  void SetStatus(LineStatus p_LineStatus);
  LineStatus GetStatus();

protected:
  LineStatus m_LineStatus;
  SimpleString* m_pLineStr;
};

#endif

