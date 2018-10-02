/* DiffFilePartition.h                                               */
/*-------------------------------------------------------------------*/
/* Class for storing a file line by line with diff information       */
/*                                                                   */
/* This has been inspired by Stephane Rodriguez source code on       */
/* CodeProject: https://www.codeproject.com/Articles/3666/Diff-tool  */
/*                                                                   */
/* Uwe Rosner                                                        */
/* 09/2018                                                           */
/*-------------------------------------------------------------------*/

#ifndef DIFF_FILE_PARTITION_H
#define DIFF_FILE_PARTITION_H

#include "LinkedList.h"
#include "SimpleString.h"

#include "DiffLine.h"

class DiffFilePartition
{
  // Constructor
public:
  DiffFilePartition(LinkedList* p_pLinesList);
  virtual ~DiffFilePartition();

  size_t NumberOfLines();
  SimpleString* GetIndexedLine(size_t idx);
  LinkedList* TokensList();

  bool PreProcess();
  bool MatchLine(size_t i1, DiffFilePartition* p_pOtherFile, size_t i2);

  void AddString(SimpleString* p_pString, DiffLine::LineStatus p_LineStatus);
  void AddString(SimpleString* p_pString);
  void AddBlankLine();

private:
  LinkedList* m_pInputLinesList;
  LinkedList* m_pDiffLinesList;
  LinkedList* m_pTokensList;

  void clearDiffLinesList();
  void clearTokensList();


//   virtual ~FilePartition();
// 
//   // Accessors
// public:
//   void SetName(SimpleString );
//   SimpleString GetName();
// 
//   void SetOptions(FileOptions &options);
// 
//   long GetNBLines(); // amount of lines of this file
//   SimpleString GetLine(long i); // returns an arbitrary line
//   SimpleString GetRawLine(long i); // returns an arbitrary line (not filtered)
//   LineStatus GetStatusLine(long i); // returns the status of an arbitrary lin
//   long *GetTokens(); // token table
// 
//   void HowManyChanges(int nAdded, int nChanged, int nDeleted);
// 
// 
//   // Methods
//   bool PreProcess(SimpleString &szFilename, FileOptions &options);
//   bool PreProcessFolder(SimpleString &szFolder, FileOptions &options);
// 
//   void AddString(SimpleString &s, LineStatus ls);
//   void AddBlankLine();
// 
//   bool MatchLine(long i1, FilePartition &f2,  long &i2);
// 
//   void Dump(SimpleString &szTitle);
// 
// 
//   // Helpers
// protected:
//   bool PreProcessFolder(SimpleString &szFolder, FileOptions &options, /*in/out*/long nIndent);
//   void AddString(SimpleString &s);
};

#endif
