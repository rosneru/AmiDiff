#include "DiffLine.h"

DiffLine::DiffLine()
  : m_LineState(Normal),
    m_Token(0)
{
}

DiffLine::~DiffLine()
{
}

void DiffLine::SetLine(const SimpleString& p_String)
{
  m_LineText = p_String;

  const char* pBuf = p_String.C_str();
  m_Token = 0;

  for(size_t i = 0; i < p_String.Length(); i++)
  {
    m_Token += 2 * m_Token + *(pBuf++);
  }
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

unsigned long DiffLine::Token() const
{
  return m_Token;
}
