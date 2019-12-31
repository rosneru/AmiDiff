#include <string.h>
#include "DiffLine.h"

DiffLine::DiffLine(const char* pText)
  : m_Text(pText),
    m_TextLength(strlen(pText)),
    m_State(Normal),
    m_pFormattedLineNumber(NULL),
    m_Token(0),
    m_bIsLinked(false)
{
  const char* pBuf = pText;
  for(size_t i = 0; i < m_TextLength; i++)
  {
    m_Token += 2 * m_Token + *(pBuf++); // (George V. Reilly hint)
  }
}

DiffLine::DiffLine(const char* pText,
                   LineState state,
                   const char* pFormattedLineNumber)
  : m_Text(pText),
    m_TextLength(strlen(pText)),
    m_State(state),
    m_pFormattedLineNumber(pFormattedLineNumber),
    m_Token(0),
    m_bIsLinked(true)
{
}

DiffLine::~DiffLine()
{
}

const char* DiffLine::Text() const
{
  return m_Text;
}

bool DiffLine::IsLinked() const
{
  return m_bIsLinked;
}

size_t DiffLine::NumChars() const
{
  return m_TextLength;
}

DiffLine::LineState DiffLine::State() const
{
  return m_State;
}

const char*DiffLine::LineNum() const
{
  return m_pFormattedLineNumber;
}

void DiffLine::SetLineNum(const char* pFormattedLineNum)
{
  m_pFormattedLineNumber = pFormattedLineNum;
}

void DiffLine::SetState(DiffLine::LineState state)
{
  m_State = state;
}

unsigned long DiffLine::Token() const
{
  return m_Token;
}
