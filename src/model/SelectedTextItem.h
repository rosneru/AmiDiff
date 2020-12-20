#ifndef SELECTED_TEXT_ITEM_H
#define SELECTED_TEXT_ITEM_H

#include <exec/types.h>

class SelectedTextItem
{
public:
  SelectedTextItem(ULONG fromLine, 
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
