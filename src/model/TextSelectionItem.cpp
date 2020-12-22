#include "TextSelectionItem.h"

TextSelectionItem::TextSelectionItem(ULONG fromLine, 
                                   ULONG toLine, 
                                   ULONG fromColumn, 
                                   ULONG toColumn)
  : m_FromLine(fromLine), 
    m_ToLine(toLine),
    m_FromColumn(fromColumn),
    m_ToColumn(toColumn)
{

}

ULONG TextSelectionItem::FromLine() const
{
  return m_FromLine;
}

ULONG TextSelectionItem::ToLine() const
{
  return m_ToLine;
}

ULONG TextSelectionItem::FromColumn() const
{
  return m_FromColumn;
}

ULONG TextSelectionItem::ToColumn() const
{
  return m_ToColumn;
}
