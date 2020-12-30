#include "TextSelectionItem.h"

TextSelectionItem::TextSelectionItem(unsigned long fromLine, 
                                     unsigned long toLine, 
                                     unsigned long fromColumn, 
                                     unsigned long toColumn)
  : m_FromLine(fromLine), 
    m_ToLine(toLine),
    m_FromColumn(fromColumn),
    m_ToColumn(toColumn)
{

}

unsigned long TextSelectionItem::FromLine() const
{
  return m_FromLine;
}

unsigned long TextSelectionItem::ToLine() const
{
  return m_ToLine;
}

unsigned long TextSelectionItem::FromColumn() const
{
  return m_FromColumn;
}

unsigned long TextSelectionItem::ToColumn() const
{
  return m_ToColumn;
}
