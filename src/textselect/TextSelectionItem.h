#ifndef TEXT_SELECTION_ITEM_H
#define TEXT_SELECTION_ITEM_H

/**
 * Represents a block of selected text.
 *
 * @author Uwe Rosner
 * @date 20/12/2020
 */
class TextSelectionItem
{
public:
  TextSelectionItem(unsigned long fromLine, 
                    unsigned long toLine, 
                    unsigned long fromColumn, 
                    unsigned long toColumn);

  unsigned long FromLine() const;
  unsigned long ToLine() const;
  unsigned long FromColumn() const;
  unsigned long ToColumn() const;

private:
  unsigned long m_FromLine;
  unsigned long m_ToLine;
  unsigned long m_FromColumn;
  unsigned long m_ToColumn;

};


#endif
