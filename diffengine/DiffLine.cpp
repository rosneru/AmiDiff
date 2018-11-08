#include "DiffLine.h"

DiffLine::DiffLine()
  : m_LineState(Normal),
    m_pLineText(NULL)
{
}

DiffLine::~DiffLine()
{
  if(m_pLineText != NULL)
  {
    delete m_pLineText;
    m_pLineText = NULL;
  }
}

long DiffLine::SetLine(const SimpleString* p_String)
{
  if(m_pLineText != NULL)
  {
    delete m_pLineText;
    m_pLineText = NULL;
  }

  m_pLineText = new SimpleString();

  // Deep-copy of p_pString
  // TODO Would it go better?
  *m_pLineText = *p_String;

  const char* pBuf = p_String->C_str();

  long nToken = 0;
  for(size_t i = 0; i < p_String->Length(); i++)
  {
    nToken += 2 * nToken + *(pBuf++); // (George V. Reilly hint)
  }

  return nToken;
}

void DiffLine::SetLine(const SimpleString* p_pString, DiffLine::LineState p_LineState)
{
  if(m_pLineText != NULL)
  {
    delete m_pLineText;
    m_pLineText = NULL;
  }

  m_pLineText = new SimpleString();

  // Deep-copy of p_pString
  // TODO Would it go better?
  if(p_pString != NULL)
  {
    *m_pLineText = *p_pString;
  }

  SetState(p_LineState);
}

const SimpleString* DiffLine::GetLine() const
{
  return m_pLineText;
}

void DiffLine::SetState(DiffLine::LineState p_LineState)
{
  m_LineState = p_LineState;
}

DiffLine::LineState DiffLine::GetState()
{
  return m_LineState;
}
