#ifndef TEXT_SELECTION_H
#define TEXT_SELECTION_H

#include <vector>

#include "TextSelectionItem.h"

/**
 * Represents multiple blocks of selected text and exposes related
 * methods.
 *
 * @author Uwe Rosner
 * @date 22/12/2020
 */
class TextSelection
{
public:
  TextSelection();
  virtual ~TextSelection();
  void Add(unsigned long fromLine, unsigned long toLine, unsigned long fromColumn, unsigned long toColumn);

private:
  std::vector<TextSelectionItem*> m_Selections;
};

#endif
