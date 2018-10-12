/* DiffEngine.h                                                      */
/*-------------------------------------------------------------------*/
/* Class for generating a diff of the contents of two files          */
/*                                                                   */
/* This is an adaption of Stephane Rodriguez source code on          */
/* CodeProject: https://www.codeproject.com/Articles/3666/Diff-tool  */
/*                                                                   */
/* Uwe Rosner                                                        */
/* 09/2018                                                           */
/*-------------------------------------------------------------------*/
#ifndef DIFF_ENGINE_H
#define DIFF_ENGINE_H

#include "SimpleString.h"
#include "DiffLine.h"
#include "DiffFilePartition.h"


class DiffEngine
{

public:
  DiffEngine();

  bool Diff(DiffFilePartition& p_File1Src,
            DiffFilePartition& p_File2Src,
            DiffFilePartition& p_File1Diff,
            DiffFilePartition& p_File2Diff);
};

#endif
