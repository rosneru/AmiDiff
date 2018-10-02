/* DiffEngine.h                                                      */
/*-------------------------------------------------------------------*/
/* Class for generating a diff of the contents of two files          */
/*                                                                   */
/* This has been inspired by Stephane Rodriguez source code on       */
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

  bool Diff(DiffFilePartition* p_File1Src,
            DiffFilePartition* p_File2Src,
            DiffFilePartition* p_File1Diff,
            DiffFilePartition* p_File2Diff);


//  // Methods
//
//  // compare f1 (old version) with f2 (new version)
//  // and build two new copies of those file objects with status on a line by line basis
//  //
//  bool Diff(  FilePartition &f1, FilePartition &f2,
//        FilePartition &f1_bis, FilePartition &f2_bis);
//
//  // build html report
//  //
//  void SetTitles(SimpleString &szHeader, SimpleString &szFooter);
//  void SetColorStyles(SimpleString &szText, SimpleString &szBackground, SimpleString &szChanged, SimpleString &szAdded, SimpleString &szDeleted);
//
//  SimpleString Serialize(  FilePartition &f1,
//            FilePartition &f2);
//
//  bool ExportAsHtml(SimpleString &szFilename, SimpleString &szContent);
//  bool ExportAsStdout(SimpleString &szContent);
//
//protected:
//  SimpleString m_szColorText, m_szColorBackground, m_szColorChanged, m_szColorAdded, m_szColorDeleted; // used by the html renderer
//  SimpleString m_szHeader, m_szFooter;
//  SimpleString Escape(SimpleString &s);
};

#endif
