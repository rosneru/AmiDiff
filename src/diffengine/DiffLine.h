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

  DiffLine(const char* pText);
  DiffLine(const char* pText, LineState state);
  ~DiffLine();

  /**
   * Returns the text of the diff line
   */
  const char* Text() const;

  /**
   * Sets the text pointer to NULL. Can be done as remark that text
   * already has been deleted.
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
