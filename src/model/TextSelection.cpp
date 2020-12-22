#include "TextSelection.h"

TextSelection::TextSelection()
{
}

TextSelection::~TextSelection()
{
  std::vector<TextSelectionItem*>::iterator it;
  for(it = m_Selections.begin(); it != m_Selections.end(); it++)
  {
    delete *it;
  }
}


void TextSelection::Add(ULONG fromLine, 
                        ULONG toLine, 
                        ULONG fromColumn, 
                        ULONG toColumn)
{
  TextSelectionItem* pSelection = new TextSelectionItem(fromLine, 
                                                        toLine, 
                                                        fromColumn, 
                                                        toColumn);

  m_Selections.push_back(pSelection);
}
