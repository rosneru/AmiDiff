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
#ifndef DIFF_ENGINE
#define DIFF_ENGINE

#include "SimpleString.h"

class DiffEngine
{

public:
	DiffEngine();

	// Methods

	// compare f1 (old version) with f2 (new version)
	// and build two new copies of those file objects with status on a line by line basis
	//
  bool Diff(	FilePartition &f1, FilePartition &f2,
				FilePartition &f1_bis, FilePartition &f2_bis);

	// build html report
	//
	void SetTitles(SimpleString &szHeader, SimpleString &szFooter);
	void SetColorStyles(SimpleString &szText, SimpleString &szBackground, SimpleString &szChanged, SimpleString &szAdded, SimpleString &szDeleted);

	SimpleString Serialize(	FilePartition &f1, 
						FilePartition &f2);

	BOOL ExportAsHtml(SimpleString &szFilename, SimpleString &szContent);
	BOOL ExportAsStdout(SimpleString &szContent);

protected:
	SimpleString m_szColorText, m_szColorBackground, m_szColorChanged, m_szColorAdded, m_szColorDeleted; // used by the html renderer
	SimpleString m_szHeader, m_szFooter;
	SimpleString Escape(SimpleString &s);
};

#endif
