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


class FileLine
{
public:
  FileLine();
  
  enum LineStatus
  {
    Normal=0,
    Changed,
    Added,
    Deleted
  };

//  long SetLine(SimpleString s); // store string and build token
//  void SetLine(SimpleString &s, LineStatus ls); // store string and status (does not eval token)
//  SimpleString GetLine(FileOptions &o);

//  void SetStatus(LineStatus ls);
//  LineStatus Status();

//protected:
//  SimpleString m_s;
//  LineStatus m_status;
//  SimpleString GetLineWithOptions(SimpleString s,FileOptions &o); // that's where options are implemented
};

#endif

