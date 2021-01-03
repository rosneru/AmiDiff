#include "TextSelectionBlock.h"

TextSelectionBlock::TextSelectionBlock(unsigned long fromColumn,
                                       unsigned long toColumn)
  : m_FromColumn(fromColumn),
    m_ToColumn(toColumn)
{

}

unsigned long TextSelectionBlock::getFromColumn() const
{
  return m_FromColumn;
}

unsigned long TextSelectionBlock::getToColumn() const
{
  return m_ToColumn;
}

unsigned long TextSelectionBlock::getNumMarkedChars(unsigned long columnId)
{
  if(columnId < m_FromColumn)
  {
    return 0;
  }

  if(columnId >= m_ToColumn)
  {
    return 0;
  }

  return m_ToColumn - columnId;
}
