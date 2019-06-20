#include "DiffLine.h"

DiffLine::DiffLine()
  : m_LineState(Normal)
{
}

DiffLine::~DiffLine()
{
}

unsigned long DiffLine::SetLine(const SimpleString& p_String)
{
  m_LineText = p_String;

  const char* pBuf = p_String.C_str();
  unsigned long nToken = 0;

  for(size_t i = 0; i < p_String.Length(); i++)
  {
    nToken += 2 * nToken + *(pBuf++); // (George V. Reilly hint)
  }

  return nToken;
}

void DiffLine::SetLine(const SimpleString& p_String, 
                       DiffLine::LineState p_LineState)
{
  m_LineText = p_String;
  m_LineState = p_LineState;
}

const SimpleString& DiffLine::Text() const
{
  return m_LineText;
}

DiffLine::LineState DiffLine::State() const
{
  return m_LineState;
}
