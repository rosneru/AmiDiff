#include <string.h>
#include "DiffLine.h"

DiffLine::DiffLine(const char* pText)
  : m_Text(pText),
    m_Length(strlen(pText)),
    m_State(Normal),
    m_Token(0),
    m_bLinkedText(false)
{
  const char* pBuf = pText;
  for(size_t i = 0; i < m_Length; i++)
  {
    m_Token += 2 * m_Token + *(pBuf++); // (George V. Reilly hint)
  }
}

DiffLine::DiffLine(const char* pText, LineState p_LineState)
  : m_Text(pText),
    m_Length(strlen(pText)),
    m_State(p_LineState),
    m_Token(0),
    m_bLinkedText(true)
{
}

DiffLine::~DiffLine()
{
}

const char* DiffLine::Text() const
{
  return m_Text;
}

bool DiffLine::TextIsLinked() const
{
  return m_bLinkedText;
}

size_t DiffLine::Length() const
{
  return m_Length;
}

DiffLine::LineState DiffLine::State() const
{
  return m_State;
}

void DiffLine::SetState(DiffLine::LineState state)
{
  m_State = state;
}

unsigned long DiffLine::Token() const
{
  return m_Token;
}
