/* DiffFilePartition.h                                               */
/*-------------------------------------------------------------------*/
/* Class for storing a file line by line with diff information       */
/*                                                                   */
/* This is an adaption of Stephane Rodriguez source code on          */
/* CodeProject: https://www.codeproject.com/Articles/3666/Diff-tool  */
/*                                                                   */
/* Uwe Rosner                                                        */
/* 09/2018                                                           */
/*-------------------------------------------------------------------*/

#ifndef DIFF_FILE_PARTITION
#define DIFF_FILE_PARTITION

#include "SimpleString.h"

typedef enum {Normal=0, Changed, Added, Deleted} LineStatus;

class FileLine; // forward declaration
class FileOptions; // forward declaration
class FilePartition; // forward declaration


class FileLine
{
public:
	FileLine();

	long SetLine(SimpleString &s, FileOptions &o); // store string and build token
	void SetLine(SimpleString &s, LineStatus ls); // store string and status (does not eval token)
	SimpleString GetLine(FileOptions &o);

	void SetStatus(LineStatus ls);
	LineStatus GetStatus();

protected:
	SimpleString m_s;
	LineStatus m_status;
	SimpleString GetLineWithOptions(SimpleString s,FileOptions &o); // that's where options are implemented
};


typedef CTemplateArray<FileLine*>		ArrayLines;
typedef CTemplateArray<long>			ArrayLong;
typedef CTemplateObjArray<SimpleString>		ArraySimpleString;


class FileOptions
{
  // Members
protected:
  ArraySimpleString arrOptions;

  // "indent" : should the diff engine see differences in tabs/spaces (default = yes)
  // "case" : should the diff engine see case differences (default = yes)

  // how to use this class :
  //       FileOptions o;
  //       o.SetOption("indent","yes");

  // Constructor
public:
  FileOptions();

  // Accessors
public:
  void SetOption(SimpleString &szOptionName, SimpleString &szOptionValue);
  SimpleString GetOption(SimpleString &szOptionName);

  bool IsDefaultOptions();
		
  long GetOptionCount();
  SimpleString GetOption(long i);

  // Methods
  void Copy(FileOptions &src);
};




class FilePartition
{

	// Members
protected:
	SimpleString			m_szFilename;
//	FileOptions	m_options;
	ArrayLines		m_arrLines;
	ArrayLong		m_arrTokens;

	// Constructor
public:
	FilePartition();
	virtual ~FilePartition();

	// Accessors
public:
	void SetName(SimpleString &);
	SimpleString GetName();

	void SetOptions(FileOptions &options);

	long GetNBLines(); // amount of lines of this file
	SimpleString GetLine(long i); // returns an arbitrary line
	SimpleString GetRawLine(long i); // returns an arbitrary line (not filtered)
	LineStatus GetStatusLine(long i); // returns the status of an arbitrary lin
	long *GetTokens(); // token table

	void HowManyChanges(int &nAdded, int &nChanged, int &nDeleted);


	// Methods
  bool PreProcess(SimpleString &szFilename, FileOptions &options);
  bool PreProcessFolder(SimpleString &szFolder, FileOptions &options);

	void AddString(SimpleString &s, LineStatus ls);
	void AddBlankLine();

  bool MatchLine(long i1, FilePartition &f2,  long &i2);

	void Dump(SimpleString &szTitle);


	// Helpers
protected:
  bool PreProcessFolder(SimpleString &szFolder, FileOptions &options, /*in/out*/long nIndent);
	void AddString(SimpleString &s);
};

#endif
