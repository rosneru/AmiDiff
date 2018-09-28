#include "FilePartition.h"


// FileLine ///////////////////////////////////////////////////
//
//

// Constructor
FileLine::FileLine()
{
	SetStatus(Normal);
}

// Accessors
long FileLine::SetLine(SimpleString &s, FileOptions &o) // store string and build token
{
	m_s = s;

	SimpleString so = GetLineWithOptions(s,o);
	
	long nToken = 0;
	long nLength = so.GetLength();
	TCHAR *lpString = so.GetBuffer(0);

	for (long i=0; i<nLength; i++)
		nToken += 2*nToken + *(lpString++); // (George V. Reilly hint)

	return nToken;
}

void FileLine::SetLine(SimpleString &s, LineStatus ls) // store string and status (does not eval ID)
{
	m_s = s;
	SetStatus(ls);
}


SimpleString FileLine::GetLine(FileOptions &o)
{
	if ( o.IsDefaultOptions() )
		return m_s;
	else
		return GetLineWithOptions(m_s,o);
}
	

SimpleString FileLine::GetLineWithOptions(SimpleString s,FileOptions &o)
{
	if ( o.IsDefaultOptions() )
		return s;
	
	SimpleString so = s;

	// should we take care about indentation differences ?
	SimpleString szIndentoption = o.GetOption( SimpleString("indent") );
	if ( !szIndentoption.IsEmpty() && szIndentoption!="yes")
	{
		// no, then remove all 09 and 20 chars from the begin of the string
		long nSize = so.GetLength();
		if (nSize>0)
		{
			TCHAR c;
			BOOL bIndentation = TRUE;

      int j = 0;
			for (long i=0; i<nSize && bIndentation; i++)
			{
        j = i;
				c = so.GetAt(i);
				if (c!=0x20 && c!=0x09)
					bIndentation = FALSE;
			}
			so = bIndentation ? so : so.Right( so.GetLength()+1 - j);
		}
	}

	// should we care about case differences ?
	SimpleString szCaseoption = o.GetOption( SimpleString("case") );
	if ( !szCaseoption.IsEmpty() && szCaseoption!="yes")
	{
		// no, then make sure we lower the case
		so.MakeLower();
	}

	return so;
}

void FileLine::SetStatus(LineStatus ls)
{
	m_status = ls;
}

LineStatus FileLine::GetStatus()
{
	return m_status;
}






//// FileOptions /////////////////////////////////////////////////////
////
///////////////////////////////////////////////////////////////////////
//// S.Rodriguez - Feb 2003
///////////////////////////////////////////////////////////////////////
////
////

//FileOptions::FileOptions()
//{
//}

//// Accessors
//void FileOptions::SetOption(SimpleString &szOptionName, SimpleString &szOptionValue)
//{
//	// is it a new option ?
//	long nSize = arrOptions.GetSize();
//	BOOL bFound = FALSE;
//	for (long i=0; i<nSize && !bFound; i++)
//	{
//		SimpleString s = arrOptions.GetAt(i);
//		if (!s.IsEmpty() && s.Find(szOptionName,0)==0)
//		{
//			bFound = TRUE;
//			long n = s.Find(';',0);
//			if (n>-1)
//				s = s.Left(n) + ";" + szOptionValue;
//			else
//				s = s + ";" + szOptionValue;

//			arrOptions.SetAt(i, s);
//		}
//	} // for

//	if (!bFound)
//	{
//		SimpleString s = szOptionName + ";" + szOptionValue;
//		arrOptions.Add(s);
//	}
//}

//SimpleString FileOptions::GetOption(SimpleString &szOptionName)
//{
//	long nSize = arrOptions.GetSize();
//	for (long i=0; i<nSize; i++)
//	{
//		SimpleString s = arrOptions.GetAt(i);
//		if (!s.IsEmpty() && s.Find(szOptionName,0)==0)
//		{
//			long n = s.Find(';',0);
//			if (n>-1)
//				return s.Right( s.GetLength() - (n+1) );
//			else
//				return SimpleString("");
//		}
//	} // for

//	return SimpleString("");
//}


//BOOL FileOptions::IsDefaultOptions()
//{
//	return GetOptionCount()==0;
//}

//long FileOptions::GetOptionCount()
//{
//	return arrOptions.GetSize();
//}

//SimpleString FileOptions::GetOption(long i)
//{
//	return (i>=0 && i<GetOptionCount()) ? arrOptions.GetAt(i) : SimpleString("");
//}


//// Methods
//void FileOptions::Copy(FileOptions &src)
//{
//	long nbOptions = src.GetOptionCount();
//	for (long i=0; i<nbOptions; i++)
//		arrOptions.Add( src.GetOption(i) );
//}









// FilePartition ///////////////////////////////////////////////////
//
//


FilePartition::FilePartition()
{
}

FilePartition::~FilePartition()
{
	// remember to destroy those FileLine objects
	long nSize = GetNBLines();
	for (long i=0; i<nSize; i++)
		delete m_arrLines.GetAt(i);
}

void FilePartition::SetName(SimpleString &szName)
{
	m_szFilename = szName;
}

SimpleString FilePartition::GetName()
{
	return m_szFilename;
}


void FilePartition::SetOptions(FileOptions &options)
{
	m_options.Copy(options);
}

long FilePartition::GetNBLines()
{
	return m_arrLines.GetSize();
}

SimpleString FilePartition::GetLine(long i)
{
	ASSERT( i>=0 && i<GetNBLines());

	FileLine *pLine = NULL;
	if (i>=0 && i<GetNBLines())
		pLine = m_arrLines.GetAt(i);

	return pLine ? pLine->GetLine(m_options) : SimpleString("");
}

SimpleString FilePartition::GetRawLine(long i) // returns an arbitrary line (not filtered)
{
	FileOptions o; // default options

	ASSERT( i>=0 && i<GetNBLines());

	FileLine *pLine = NULL;
	if (i>=0 && i<GetNBLines())
		pLine = m_arrLines.GetAt(i);

	return pLine ? pLine->GetLine(o) : SimpleString("");
}

LineStatus FilePartition::GetStatusLine(long i)
{
	ASSERT( i>=0 && i<GetNBLines());

	FileLine *pLine = NULL;
	if (i>=0 && i<GetNBLines())
		pLine = m_arrLines.GetAt(i);

	return pLine ? pLine->GetStatus() : Normal;
}

long *FilePartition::GetTokens()
{
	return m_arrTokens.GetData();
}


void FilePartition::HowManyChanges(int &nAdded, int &nChanged, int &nDeleted)
{
	nAdded = nChanged = nDeleted = 0;

	LineStatus currentStatus = Normal;

	FileLine *pLine = NULL;
	long nSize = GetNBLines();
	for (long i=0; i<nSize; i++)
	{
		pLine = m_arrLines.GetAt(i);
		if (!pLine) continue;

		LineStatus ls = pLine->GetStatus();
		if (ls!=Normal && currentStatus!=ls)
		{
			switch (ls)
			{
				case Added: nAdded++; break;
				case Changed: nChanged++; break;
				case Deleted: nDeleted++; break;
			}
		}
	} // end for all lines
}


// PreProcess
//
// purpose : reads a file, on a line-by-line basis
//
BOOL FilePartition::PreProcess(SimpleString &szFilename, FileOptions &options)
{
	ASSERT( !szFilename.IsEmpty() );
	if (szFilename.IsEmpty())
	{
		OutputDebugString("error : empty input filename\r\n");
		return FALSE;
	}

	SetName(szFilename);
	SetOptions(options);

	// read the file first,
	// and build the table of tokens

	CStdioFile f;
	if ( !f.Open(szFilename, CFile::modeRead) )
	{
		TCHAR szError[MAX_PATH];
		sprintf(szError, "error : cannot open %s\r\n", szFilename.GetBuffer(0) );
		OutputDebugString(szError);
		return FALSE;
	}

	// 
	SimpleString s;
	while ( f.ReadString(s) )
		AddString(s);

	f.Close();

	return TRUE;
}


// PreProcessFolder
//
// purpose : list folders, files and subfolders
//           and then build a tree-like list out of it
//
BOOL FilePartition::PreProcessFolder(SimpleString &szFolder, FileOptions &options)
{

	SetName(szFolder);
	SetOptions(options);

	long nIndent = 0;

	// make sure there is no trailing backslash
	while (!szFolder.IsEmpty() && szFolder.GetAt( szFolder.GetLength()-1 )=='\\')
		szFolder = szFolder.Left( szFolder.GetLength()-1 );

	return PreProcessFolder(szFolder, options, nIndent);
}

BOOL FilePartition::PreProcessFolder(SimpleString &szFolder, FileOptions &options, /*in/out*/long nIndent)
{
	if (szFolder.IsEmpty()) return FALSE;

	CSortedArray arrFiles;

	long i;
	SimpleString szIndent;
	for (i=0; i<nIndent; i++)
		szIndent += "  ";

	// add this folder
	SimpleString szCurrentFolder = szFolder;
	int nBackSlash = szCurrentFolder.ReverseFind('\\');
	if (nBackSlash>-1)
		szCurrentFolder = szCurrentFolder.Right( szCurrentFolder.GetLength()-(nBackSlash+1) );

	// add the folder in the partition
	AddString( szIndent + "+ " + szCurrentFolder );


	SimpleString szFolderWildCard = szFolder + "\\*.*";

	WIN32_FIND_DATA findFile;
	HANDLE hContext;
	hContext=::FindFirstFile(szFolderWildCard.GetBuffer(0), &findFile);
	if (  hContext != INVALID_HANDLE_VALUE )
	{
		do
		{
			if (strcmp(findFile.cFileName,".")!=0 
				&& strcmp(findFile.cFileName,"..")!=0)
			{

				if (findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				{
					// it's a folder
					//
					SimpleString szSubFolder = szFolder + "\\" + findFile.cFileName;
					PreProcessFolder(szSubFolder, options, nIndent+1);
				}
				else
				{
					// it's a file
					//
					// GetInformationByHandle
					
					OFSTRUCT ofstruct;
					BY_HANDLE_FILE_INFORMATION infostruct;
					HFILE hFile = ::OpenFile(szFolder + "\\" + findFile.cFileName, 
											 &ofstruct, 
											 OF_READ | OF_SHARE_DENY_NONE);
					if (hFile!=HFILE_ERROR && ::GetFileInformationByHandle( (HANDLE)hFile, &infostruct) )
					{
						SYSTEMTIME stCreate;

						// Convert the local file time from UTC to system time.
						FileTimeToSystemTime(&infostruct.ftLastWriteTime, &stCreate);

						TCHAR sDate[128];
						sprintf(sDate, "%02d/%02d/%d  %02d:%02d",
								stCreate.wDay, stCreate.wMonth, stCreate.wYear,
								stCreate.wHour, stCreate.wMinute);

						::_lclose(hFile);

						arrFiles.InsertSorted( SimpleString(findFile.cFileName + SimpleString(" (") + sDate + ")") );
					}
				}

			}
		}
		while (	::FindNextFile(hContext, &findFile) );

		::FindClose(hContext);
	}

	// add all files at this point
	long nbFiles = arrFiles.GetSize();
	for (i=0; i<nbFiles; i++)
		AddString( szIndent + "    " + arrFiles.GetAt(i) );

	return TRUE;
}


void FilePartition::AddString(SimpleString &s, LineStatus ls)
{
	FileLine *p = new FileLine();
	ASSERT(p);
	if (p)
	{
		p->SetLine(s, ls);
		m_arrLines.Add( p );
	}
}

void FilePartition::AddBlankLine()
{
	AddString(SimpleString(""), Normal);
}

void FilePartition::AddString(SimpleString &s)
{
	FileLine *p = new FileLine();
	ASSERT(p);
	if (p)
	{
		m_arrTokens.Add( p->SetLine(s, m_options) );
		m_arrLines.Add( p );
	}
}


void FilePartition::Dump(SimpleString &szTitle)
{
	TCHAR s[MAX_PATH];
	sprintf(s, "%s : %d lines\r\n\r\n", szTitle.GetBuffer(0), GetNBLines() );
	OutputDebugString(s);

	long nSize = GetNBLines();
	for (long i=0; i<nSize; i++)
	{
		LineStatus ls = GetStatusLine(i);
		if (ls==Normal) 
			OutputDebugString("Normal ");
		else if (ls==Changed)
			OutputDebugString("Changed ");
		else if (ls==Added)
			OutputDebugString("Added ");
		else if (ls==Deleted)
			OutputDebugString("Deleted ");

		OutputDebugString( GetLine(i) );
		OutputDebugString("\r\n");
	}
}

BOOL FilePartition::MatchLine(long i1, FilePartition &f2, long &i2)
{
	ASSERT( m_arrTokens.GetSize()>0 );
	if ( m_arrTokens.GetSize()==0 ) return FALSE;

	ASSERT( i1<GetNBLines() );
	if ( i1>=GetNBLines() ) return FALSE; // should never happen though

	// try to match the current token from f1, into the a subset of f2 tokens
	//
	
	long nf1Token;
	nf1Token = m_arrTokens[i1];

	long *pf2Tokens = f2.GetTokens();
	if (!pf2Tokens)
		return FALSE;

	BOOL bFound = FALSE;
	long i = 0;
	long nf2SubsetLines = f2.GetNBLines() - i2;

	long *pf2Token = pf2Tokens + i2; // points to the right token

	while (!bFound && i<nf2SubsetLines)
	{
		if (nf1Token == *pf2Token) // fast compare
		{
			// make sure strings really match
			bFound = ( GetLine(i1).Compare( f2.GetLine(i2+i) )==0 ); 
		}

		pf2Token++;
		i++;
	}
	i--;

	if (bFound)
	{
		i2 += i;
		return TRUE;
	}

	return FALSE;
}
