#ifndef TEXT_SELECTION_BLOCK_H
#define TEXT_SELECTION_BLOCK_H

/**
 * Represents a block selected text (on a line).
 *
 * @author Uwe Rosner
 * @date 03/01/2021
 */
class TextSelectionBlock
{
public:
  TextSelectionBlock(unsigned long fromColumn, 
                     unsigned long toColumn);

  unsigned long getFromColumn() const;
  unsigned long getToColumn() const;

  unsigned long getNumMarkedChars(unsigned long columnId);

private:
  unsigned long m_FromColumn;
  unsigned long m_ToColumn;
};

#endif
