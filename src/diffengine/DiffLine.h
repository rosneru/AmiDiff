#ifndef DIFF_LINE_H
#define DIFF_LINE_H

#include <stdlib.h>

/**
 * Represents a line in a Diff which contains of Text and a LineState.
 *
 * It originates from Stephane Rodriguez open diff implementation:
 * https://www.codeproject.com/Articles/3666/Diff-tool
 *
 * @author Uwe Rosner
 * @date 28/09/2018
 */
class DiffLine
{
public:
  enum LineState
  {
    Normal = 0,
    Changed,
    Added,
    Deleted,
    Undefined
  };

  /**
   * Creates a new DiffLine with the given text.
   *
   * IMPORTANT: Sets the IsLinked flag to false so that this DiffLine
   * is considered as 'owner' of the given text and line number.
   */
  DiffLine(const char* pText);

  /**
   * Creates a new DiffLine with given text and state.
   *
   * IMPORTANT: Sets the IsLinked flag to true so that another DiffLine
   * is considered as 'owner' of the given text.
   */
  DiffLine(const char* pText,
           LineState state,
           const char* pLineNumberText);


  virtual ~DiffLine();

  /**
   * Returns the text of the diff line
   */
  const char* getText() const;


  /**
   * Returns the line length in number of chars.
   */
  size_t getNumChars() const;

  /**
   * Return the state of the this line
   */
  LineState getState() const;

  /**
   * Set the state of this line.
   */
  void setState(LineState state);

  /**
   * Return the LineNumber as a digit-formatted text.
   *
   * E.g. when highest line number is three-digits long, the returned
   * value will be something like "  1", " 82" or "123".
   *
   * Returns NULL if collecting the line numbers was't enabled in the
   * corresponding src DiffFile before PreProcess().
   */
  const char* getLineNumText() const;

  /**
   *  Set the digit-formatted number for this line.
   */
  void setLineNumText(const char* pLineNum);

  /**
   * Returns the token which has been calculated as a numerical
   * representation of this lines text context
   */
  unsigned long getToken() const;

protected:
  const char* m_Text;
  const size_t m_TextLength;
  LineState m_State;
  const char* m_pLineNumberText;
  unsigned long m_Token;
  bool m_bIsLinked;
};

#endif
