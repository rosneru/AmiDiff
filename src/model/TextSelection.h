#ifndef TEXT_SELECTION_H
#define TEXT_SELECTION_H

#include <vector>

#include "TextSelectionItem.h"

class TextSelection
{
public:
  TextSelection();
  virtual ~TextSelection();
  void Add(ULONG fromLine, ULONG toLine, ULONG fromColumn, ULONG toColumn);

private:
  std::vector<TextSelectionItem*> m_Selections;
};

#endif
