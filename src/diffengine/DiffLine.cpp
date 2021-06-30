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



TextPositionInfo DiffLine::getTextPositionInfo(unsigned long resultingTextColumn,
                                               unsigned long tabWidth) const
{
  TextPositionInfo info = {0};
  unsigned long i, accumulatedColumn, tabIndent;

  accumulatedColumn = 0;

  // Parse each character of input text
  for(info.srcTextColumn = 0; info.srcTextColumn < m_TextLength; info.srcTextColumn++)
  {
    if(accumulatedColumn >= resultingTextColumn)
    {
      tabIndent = tabWidth - (size_t)(resultingTextColumn % tabWidth);

      if(accumulatedColumn > resultingTextColumn)
      {
        // In midst of / among a tabulator block
        info.numRemainingChars = 0;
        info.numRemainingSpaces = tabIndent;
      }
      else
      {
        if(m_Text[info.srcTextColumn] == '\t')
        {
          // Directly on the start of a tabulator block
          info.numRemainingChars = 0;
          info.numRemainingSpaces = tabIndent;
        }
        else
        {
          // A printable character, no tabulator block
          
          // Check how many chars / spaces until next tab position or eol
          for(i = info.srcTextColumn; i < m_TextLength; i++)
          {
            if(m_Text[i] == '\t')
            {
              break;
            }
          }

          info.numRemainingChars = i - info.srcTextColumn;
          info.numRemainingSpaces = 0;
        }
      }

      return info;
    }

    if(m_Text[info.srcTextColumn] == '\t')
    {
      // Increase actual result column by current position tabulator indent
      accumulatedColumn += (size_t)( tabWidth - (accumulatedColumn % tabWidth));
    }
    else
    {
      // Increase actual result column by one
      accumulatedColumn++;
    }
  }

  return info;
}
