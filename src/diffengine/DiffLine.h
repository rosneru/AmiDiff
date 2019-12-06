#ifndef DIFF_LINE_H
#define DIFF_LINE_H

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
   * is considered as 'owner' of the given text.
   */
  DiffLine(const char* pText);

  /**
   * Creates a new DiffLine with given text and state.
   *
   * IMPORTANT: Sets the IsLinked flag to true so that another DiffLine
   * is considered as 'owner' of the given text.
   */
  DiffLine(const char* pText, LineState state);

  ~DiffLine();

  /**
   * Returns the text of the diff line
   */
  const char* Text() const;

  /**
   * Returns true if this line's text is only a flat copy of another
   * diff line's text.
   */
  bool TextIsLinked() const;

  /**
   * Returns the text length of the diff line
   */
  size_t Length() const;

  /**
   * Returns the state of the diff line
   */
  LineState State() const;

  void SetState(LineState state);

  /**
   * Returns the token which is asociated with this line
   */
  unsigned long Token() const;

protected:
  const char* m_Text;
  const size_t m_Length;
  LineState m_State;
  unsigned long m_Token;
  bool m_bLinkedText;
};

#endif
