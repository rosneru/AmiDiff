#include "DiffLine.h"

DiffLine::DiffLine()
  : m_LineStatus(Normal),
    m_pLineStr(NULL)
{
}

DiffLine::~DiffLine()
{
  if(m_pLineStr != NULL)
  {
    delete m_pLineStr;
    m_pLineStr = NULL;
  }
}

long DiffLine::SetLine(SimpleString* p_pString)
{
  if(m_pLineStr != NULL)
  {
    delete m_pLineStr;
    m_pLineStr = NULL;
  }

  m_pLineStr = new SimpleString();

  // Deep-copy of p_pString
  // TODO Would it go better?
  *m_pLineStr = *p_pString;

  char* pBuf = p_pString->C_str();

  long nToken = 0;
  for(size_t i = 0; i < p_pString->Length(); i++)
  {
    nToken += 2 * nToken + *(pBuf++); // (George V. Reilly hint)
  }

  return nToken;
}

void DiffLine::SetLine(SimpleString* p_pString, DiffLine::LineStatus p_LineStatus)
{
  if(m_pLineStr != NULL)
  {
    delete m_pLineStr;
    m_pLineStr = NULL;
  }

  m_pLineStr = new SimpleString();

  // Deep-copy of p_pString
  // TODO Would it go better?
  *m_pLineStr = *p_pString;
  SetStatus(p_LineStatus);
}

SimpleString* DiffLine::GetLine()
{
  return m_pLineStr;
}

void DiffLine::SetStatus(DiffLine::LineStatus p_LineStatus)
{
  m_LineStatus = p_LineStatus;
}

DiffLine::LineStatus DiffLine::GetStatus()
{
  return m_LineStatus;
}
