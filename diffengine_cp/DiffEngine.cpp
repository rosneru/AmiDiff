#include "DiffEngine.h"

DiffEngine::DiffEngine()
{
  m_szColorText    = "#888888"; // default colors
  m_szColorBackground = "white";
  m_szColorChanged  = "#FFFFBB";
  m_szColorAdded    = "#BBFFBB";
  m_szColorDeleted  = "#FFBBBB";

  m_szFooter = "<br><font size='-2' color='#BBBBBB'>S.Rodriguez, May 2003</font>";
}

// compare f1 (old version) with f2 (new version)
// and build two new copies of those file objects with status on a line by line basis
//
bool DiffEngine::Diff(  FilePartition &f1, FilePartition &f2,
            FilePartition &f1_bis, FilePartition &f2_bis)
{
  f1_bis.SetName( f1.GetName() );
  f2_bis.SetName( f2.GetName() );

  long nbf1Lines = f1.GetNBLines();
  long nbf2Lines = f2.GetNBLines();

  // special empty file case
  if ( nbf1Lines==0 )
  {
    long nLinef2 = 0;
    SimpleString s;
    while ( nLinef2<nbf2Lines )
    {
      f1_bis.AddBlankLine();
      f2_bis.AddString( f2.GetRawLine(nLinef2++), Normal);
    }
    return true;
  }

  long i = 0;
  long nf2CurrentLine = 0;

  while ( i<nbf1Lines )
  {
    // process this line (and possibly update indexes as well)
    long nLinef2 = nf2CurrentLine;

    if ( nLinef2 >= nbf2Lines )
    {
      // it's time to end the game now
      while ( i < nbf1Lines )
      {
        f1_bis.AddString( f1.GetRawLine(i), Deleted);
        f2_bis.AddBlankLine();
        i++;
      }
      break;
    }

    if ( f1.MatchLine(i, f2, nLinef2) )
    {
      bool bDeleted = false;
      if (nLinef2 > nf2CurrentLine)
      {
        long itmp = nf2CurrentLine;
        bDeleted = f2.MatchLine(nf2CurrentLine, f1, itmp) && (itmp<nLinef2);
        if (bDeleted)
        {
          long j = itmp - i;
          while ( j>0 )
          {
            f1_bis.AddString( f1.GetRawLine(i), Deleted);
            f2_bis.AddBlankLine();

            i++;
            j--;
          }
          // please note nf2CurrentLine is not updated

          continue; // jump here to loop iteration
        }
      }

      // matched, so either the lines were identical, or f2 has added one or more lines
      if (nLinef2 > nf2CurrentLine)
      {
        // add blank lines to f1_bis
        long j = nLinef2 - nf2CurrentLine;
        while ( j>0 )
        {
          f1_bis.AddBlankLine();
          f2_bis.AddString( f2.GetRawLine(nLinef2-j), Added );

          j--;
        }
      }

      // exactly matched
      f1_bis.AddString( f1.GetRawLine(i), Normal);
      f2_bis.AddString( f2.GetRawLine(nLinef2), Normal);

      nf2CurrentLine = nLinef2 + 1; // next line in f2

    }
    else
    {
      // this line is not found at all in f2, either it's because it has been changed, or even deleted
      long nLinef1 = i;
      if ( f2.MatchLine(nLinef2, f1, nLinef1) )
      {
        // the dual line in f2 can be found in f1, that's because
        // the current line in f1 has been deleted
        f1_bis.AddString( f1.GetRawLine(i), Deleted);
        f2_bis.AddBlankLine();

        // this whole block is flagged as deleted
        if (nLinef1>i+1)
        {
          long j = nLinef1 - (i+1);
          while ( j>0 )
          {
            i++;

            f1_bis.AddString( f1.GetRawLine(i), Deleted);
            f2_bis.AddBlankLine();

            j--;
          }
        }

        // note : nf2CurrentLine is not incremented
      }
      else
      {
        // neither added, nor deleted, so it's flagged as changed
        f1_bis.AddString( f1.GetRawLine(i), Changed);
        f2_bis.AddString( f2.GetRawLine(nLinef2), Changed);

        nf2CurrentLine = nLinef2 + 1; // next line in f2
      }
    }

    i++; // next line in f1
  }

  // are there any remaining lines from f2?
  while ( nf2CurrentLine < nbf2Lines )
  {
    f1_bis.AddBlankLine();
    f2_bis.AddString( f2.GetRawLine(nf2CurrentLine), Added );

    nf2CurrentLine++;
  }


  return true;
}







// build html report
//



void DiffEngine::SetTitles(SimpleString &szHeader, SimpleString &szFooter)
{
  m_szHeader = szHeader;
  m_szFooter = szFooter;
}

void DiffEngine::SetColorStyles(SimpleString &szText, SimpleString &szBackground, SimpleString &szChanged, SimpleString &szAdded, SimpleString &szDeleted)
{
  m_szColorText = szText;
  m_szColorBackground = szBackground;
  m_szColorChanged = szChanged;
  m_szColorAdded = szAdded;
  m_szColorDeleted = szDeleted;
}

SimpleString DiffEngine::Serialize(  FilePartition &f1,
                FilePartition &f2)
{

  // eval amount of differences between the two files
  int nAdded_f1, nChanged_f1, nDeleted_f1;
  f1.HowManyChanges(nAdded_f1, nChanged_f1, nDeleted_f1);
  int nAdded_f2, nChanged_f2, nDeleted_f2;
  f2.HowManyChanges(nAdded_f2, nChanged_f2, nDeleted_f2);

  int nTotal = nAdded_f1 + nDeleted_f1 + nChanged_f1 + nAdded_f2 + nDeleted_f2;

  if (nTotal==0)
    m_szHeader += "<font size=-1><b>Files are identical.</font><br>";
  else
  {
    TCHAR szTmp[128];
    sprintf(szTmp,"<font size=-1><b>%d changes between the files.</font><br>", nTotal);
    m_szHeader += szTmp;
  }



  // write html header
  SimpleString s = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\r\n" \
        "<!-- diff html gen, (c) Stephane Rodriguez - feb 2003 -->\r\n" \
        "<HTML>\r\n" \
        "<HEAD>\r\n" \
        "<TITLE> File Diff </TITLE>\r\n" \
        "<style type='text/css'>\r\n"\
        "<!--\r\n" \
        ".N { background-color:white; }\r\n" \
        ".C { background-color:" + m_szColorChanged + "; }\r\n" \
        ".A { background-color:" + m_szColorAdded + "; }\r\n" \
        ".D { background-color:" + m_szColorDeleted + "; }\r\n" \
        "-->\r\n" \
        "</style>\r\n" \
        "</HEAD>\r\n" \
        "\r\n" \
        "<BODY BGCOLOR='#FFFFFF'>\r\n" \
        "\r\n" + m_szHeader + \
        "<table border=0 bgcolor=0 cellpadding=1 cellspacing=1 width=100%><tr><td>\r\n" \
        "<table width=100% bgcolor=white border=0 cellpadding=0 cellspacing=0>\r\n" \
        "<tr bgColor='#EEEEEE' style='color:0'><td width=50%>old version</td><td width=50%>new version" \
        "&nbsp;&nbsp;&nbsp;(<b style='background-color:" + m_szColorChanged + ";width:20'>&nbsp;</b>changed&nbsp;&nbsp;" \
        "<b style='background-color:" + m_szColorAdded + ";width:20'>&nbsp;</b>added&nbsp;&nbsp;" \
        "<b style='background-color:" + m_szColorDeleted + ";width:20'>&nbsp;</b>deleted)&nbsp;&nbsp;" \
        "<FORM ACTION='' style='display:inline'><SELECT id='fontoptions' " \
        "onchange='maintable.style.fontSize=this.options[this.selectedIndex].value'>" \
        "<option value='6pt'>6pt<option value='7pt'>7pt<option value='8pt'>8pt<option value='9pt' selected>9pt</SELECT> " \
        "</FORM></td></tr>\r\n" \
        "<tr bgColor='#EEEEEE' style='color:0'><td width=50%><code>" + f1.GetName() + "</code></td><td width=50%><code>" + f2.GetName() + "</code></td></tr>" \
        "</table>\r\n" \
        "</td></tr>\r\n" \
        "</table>\r\n" \
        "\r\n" \
        "<br>\r\n" \
        "\r\n" ;

  long nbLines = f1.GetNBLines();
  if (nbLines==0)
  {
    s += "<br>empty files";
  }
  else
  {
    s += "<table border=0 bgcolor=0 cellpadding=1 cellspacing=1 width=100%><tr><td>" \
      "<table id='maintable' width=100% bgcolor='" + m_szColorBackground + "' cellpadding=0 cellspacing=0 border=0 style='color:" + m_szColorText + ";font-family: Courier New, Helvetica, sans-serif; font-size: 8pt'>\r\n";
  }


  char *arrStatus[4] = {
    "",
    " class='C'",
    " class='A'",
    " class='D'" };

  SimpleString sc;

  TCHAR szLine[16];

  // write content
  //
  for (long i=0; i<nbLines; i++)
  {
    sprintf(szLine, "<b>%d</b>", i);
    sc += "<tr><td width=50%" + SimpleString(arrStatus[ f1.GetStatusLine(i) ]) + ">" +
        SimpleString(szLine) +
        " " +
        Escape(f1.GetRawLine(i)) + "</td>";
    sc += "<td width=50%" + SimpleString(arrStatus[ f2.GetStatusLine(i) ]) + ">" +
        SimpleString(szLine) +
        " " +
        Escape(f2.GetRawLine(i)) + "</td></tr>";

  } // for i

  s += sc;

  if (nbLines>0)
    s += "</table></td></tr></table>\r\n";

  // write html footer
  s += m_szFooter + "</BODY>\r\n" \
    "</HTML>\r\n";

  return s;
}



SimpleString DiffEngine::Escape(SimpleString &s) // a helper aimed to make sure tag symbols are passed as content
{
  SimpleString o;
  long nSize = s.GetLength();
  if (nSize==0) return SimpleString("&nbsp;");

  TCHAR c;
  bool bIndentation = true;

  for (long i=0; i<nSize; i++)
  {
    c = s.GetAt(i);
    if (bIndentation && (c==' ' || c=='\t'))
    {
      if (c==' ')
        o += "&nbsp;";
      else
        o += "&nbsp;&nbsp;&nbsp;&nbsp;";
      continue;
    }
    bIndentation = false;

    if (c=='<')
      o += "&lt;";
    else if (c=='>')
      o += "&gt;";
    else if (c=='&')
      o += "&amp;";
    else
      o += c;
  }
  return o;
}


bool DiffEngine::ExportAsHtml(SimpleString &szFilename, SimpleString &szContent)
{
  CStdioFile f;
  if ( !f.Open(szFilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary) )
  {
    TCHAR szError[MAX_PATH];
    sprintf(szError, "error : cannot create %s\r\n", szFilename.GetBuffer(0) );
    OutputDebugString(szError);
    return false;
  }

  f.Write(szContent.GetBuffer(0), szContent.GetLength());

  f.Close();

  return true;
}


bool DiffEngine::ExportAsStdout(SimpleString &szContent)
{
  printf ("%s", szContent.GetBuffer(0));
  return true;
}


