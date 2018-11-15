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

  DiffLine();
  ~DiffLine();

  /**
   * Store a string and build its token
   */
  long SetLine(const SimpleString p_pString);
  
  /**
   * Store a string and its state. Does not evaluate token.
   */
  void SetLine(const SimpleString& p_String, LineState p_LineState);
  
  const SimpleString& GetText() const;

  void SetState(LineState p_LineState);
  LineState GetState();

protected:
  LineState m_LineState;
  SimpleString m_LineText;
};

#endif
