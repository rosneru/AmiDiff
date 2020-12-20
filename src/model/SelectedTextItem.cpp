#include "SelectedTextItem.h"

SelectedTextItem::SelectedTextItem(ULONG fromLine, 
                                   ULONG toLine, 
                                   ULONG fromColumn, 
                                   ULONG toColumn)
  : m_FromLine(fromLine), 
    m_ToLine(toLine),
    m_FromColumn(fromColumn),
    m_ToColumn(toColumn)
{

}

ULONG SelectedTextItem::FromLine() const
{
  return m_FromLine;
}

ULONG SelectedTextItem::ToLine() const
{
  return m_ToLine;
}

ULONG SelectedTextItem::FromColumn() const
{
  return m_FromColumn;
}

ULONG SelectedTextItem::ToColumn() const
{
  return m_ToColumn;
}
