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
  enum LineState
  {
    Normal = 0,
    Changed,
    Added,
    Deleted,
    Undefined
  };

  DiffLine();
  ~DiffLine();

  long SetLine(SimpleString* p_pString); ///> store string and build token
  void SetLine(SimpleString* p_pString, LineState p_LineState); ///> store string and state (does not eval token)
  SimpleString* GetLine();

  void SetState(LineState p_LineState);
  LineState GetState();

protected:
  LineState m_LineState;
  SimpleString* m_pLineText;
};

#endif

