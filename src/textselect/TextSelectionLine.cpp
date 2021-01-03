#include "TextSelectionLine.h"

TextSelectionLine::TextSelectionLine(unsigned long lineId, 
                                     unsigned long fromColumn, 
                                     unsigned long toColumn)
  : m_LineId(lineId)
{
  addBlock(fromColumn, toColumn);
}

TextSelectionLine::~TextSelectionLine()
{
  std::vector<TextSelectionBlock*>::iterator it;
  for(it = m_SelectedBlocks.begin(); it != m_SelectedBlocks.end(); it++)
  {
    delete *it;
  }
}

void TextSelectionLine::addBlock(unsigned long fromColumn, 
                                 unsigned long toColumn)
{
  TextSelectionBlock* pBlock = new TextSelectionBlock(fromColumn, toColumn);
  m_SelectedBlocks.push_back(pBlock);
}

unsigned long TextSelectionLine::getLineId() const
{
  return m_LineId;
}

unsigned long TextSelectionLine::getNumMarkedChars(unsigned long columnId)
{
  std::vector<TextSelectionBlock*>::iterator it;
  for(it = m_SelectedBlocks.begin(); it != m_SelectedBlocks.end(); it++)
  {
    unsigned long numMarkedChars = (*it)->getNumMarkedChars(columnId);
    if(numMarkedChars > 0)
    {
      return numMarkedChars;
    }
  }

  return 0;
}
