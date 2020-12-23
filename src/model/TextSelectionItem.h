#ifndef TEXT_SELECTION_ITEM_H
#define TEXT_SELECTION_ITEM_H

#include <exec/types.h>

/**
 * Represents a block of selected text.
 *
 * @author Uwe Rosner
 * @date 20/12/2020
 */
class TextSelectionItem
{
public:
  TextSelectionItem(ULONG fromLine, 
                    ULONG toLine, 
                    ULONG fromColumn, 
                    ULONG toColumn);

  ULONG FromLine() const;
  ULONG ToLine() const;
  ULONG FromColumn() const;
  ULONG ToColumn() const;

private:
  ULONG m_FromLine;
  ULONG m_ToLine;
  ULONG m_FromColumn;
  ULONG m_ToColumn;

};


#endif
