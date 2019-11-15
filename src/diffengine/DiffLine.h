#ifndef DIFF_LINE_H
#define DIFF_LINE_H

#include "LinkedList.h"
#include "SimpleString.h"

/**
 * Represents a line in a Diff which contains of Text and a LineState.
 *
 * This is a port of Stephane Rodriguez source code on CodeProject:
 * https://www.codeproject.com/Articles/3666/Diff-tool
 *
 * In 2005 he released an updated version:
 * http://www.arstdesign.com/articles/difftool.html
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

  DiffLine(const SimpleString& p_String);
  DiffLine(const SimpleString& p_String, LineState p_LineState);
  ~DiffLine();

  /**
   * Returns the text of the diff line
   */
  const SimpleString& Text() const;

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
  SimpleString m_LineText;
  LineState m_LineState;
  unsigned long m_Token;
};

#endif
