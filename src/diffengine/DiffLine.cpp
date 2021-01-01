#include <string.h>
#include "DiffLine.h"

DiffLine::DiffLine(const char* pText)
  : m_Text(pText),
    m_TextLength(strlen(pText)),
    m_State(Normal),
    m_pLineNumberText(NULL),
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
                   const char* pLineNumberText)
  : m_Text(pText),
    m_TextLength(strlen(pText)),
    m_State(state),
    m_pLineNumberText(pLineNumberText),
    m_Token(0),
    m_bIsLinked(true)
{
}

DiffLine::~DiffLine()
{
}

const char* DiffLine::getText() const
{
  return m_Text;
}

size_t DiffLine::getNumChars() const
{
  return m_TextLength;
}

DiffLine::LineState DiffLine::getState() const
{
  return m_State;
}

const char*DiffLine::getLineNumText() const
{
  return m_pLineNumberText;
}

void DiffLine::setLineNumText(const char* pFormattedLineNum)
{
  m_pLineNumberText = pFormattedLineNum;
}

void DiffLine::setState(DiffLine::LineState state)
{
  m_State = state;
}

unsigned long DiffLine::getToken() const
{
  return m_Token;
}
