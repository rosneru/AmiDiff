#ifndef TEXT_SELECTION_H
#define TEXT_SELECTION_H

#include <vector>
#include "TextSelectionLine.h"

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
  void add(unsigned long lineId, 
           unsigned long fromColumn, 
           unsigned long toColumn);

  unsigned long getNumMarkedChars(unsigned long lineId, unsigned long columnId);

private:
  std::vector<TextSelectionLine*> m_SelectedLines;

  TextSelectionLine* findSelectionLine(unsigned long lineId);
};

#endif
