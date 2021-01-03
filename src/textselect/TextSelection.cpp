#include <stddef.h>
#include "TextSelection.h"

TextSelection::TextSelection()
{
}

TextSelection::~TextSelection()
{
  std::vector<TextSelectionLine*>::iterator it;
  for(it = m_SelectedLines.begin(); it != m_SelectedLines.end(); it++)
  {
    delete *it;
  }
}


void TextSelection::add(unsigned long lineId, 
                        unsigned long fromColumn, 
                        unsigned long toColumn)
{
  TextSelectionLine* pSelectionLine = findSelectionLine(lineId);
  if(pSelectionLine != NULL)
  {
    pSelectionLine->addBlock(fromColumn, toColumn);
  }
  else
  {
    pSelectionLine = new TextSelectionLine(lineId, fromColumn, toColumn);
    m_SelectedLines.push_back(pSelectionLine);

    // TODO sort m_SelectedLines by lineId
  }
}


unsigned long TextSelection::getNumMarkedChars(unsigned long lineId, 
                                               unsigned long columnId)
{
  std::vector<TextSelectionLine*>::iterator it;
  for(it = m_SelectedLines.begin(); it != m_SelectedLines.end(); it++)
  {
    if((*it)->getLineId() > lineId)
    {
      return 0;
    }

    if((*it)->getLineId() == lineId)
    {
      unsigned long numMarkedChars = (*it)->getNumMarkedChars(columnId);
      if(numMarkedChars > 0)
      {
        return numMarkedChars;
      }
    }
  }

  return 0;
}

TextSelectionLine* TextSelection::findSelectionLine(unsigned long lineId)
{
  std::vector<TextSelectionLine*>::iterator it;
  for(it = m_SelectedLines.begin(); it != m_SelectedLines.end(); it++)
  {
    if((*it)->getLineId() > lineId)
    {
      return NULL;
    }

    if((*it)->getLineId() == lineId)
    {
      return *it;
    }
  }

  return NULL;
}
