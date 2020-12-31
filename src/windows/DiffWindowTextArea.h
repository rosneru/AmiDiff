#ifndef DIFF_WINDOW_TEXT_AREA_H
#define DIFF_WINDOW_TEXT_AREA_H

#include <exec/types.h>
#include <graphics/text.h>
#include <intuition/intuition.h>

#include "DiffOutputFileBase.h"
#include "DiffLine.h"
#include "DiffWindowRastports.h"
#include "Rect.h"
#include "SelectableDiffFile.h"
#include "TextSelection.h"


#include <vector>


class DiffWindowTextArea : public Rect
{
public:
  DiffWindowTextArea(const DiffOutputFileBase& diffFile, 
                     DiffWindowRastports*& pRPorts,
                     struct TextFont* pTextFont,
                     bool lineNumbersEnabled,
                     ULONG maxNumChars);

  virtual ~DiffWindowTextArea();

  ULONG X() const;
  ULONG Y() const;

  ULONG MaxVisibleChars() const;
  ULONG MaxVisibleLines() const;

  /**
   * Set the dimensions of this text area.
   */
  void SetWidthHeight(unsigned long width, unsigned long height);

  /**
   * Scroll the text up or down to diplay the given (by rowId) line on
   * firts position top.
   */
  void ScrollTopToRow(ULONG rowId);

  /**
   * Scroll the text left or right to diplay the given (by column) line
   * on first position left.
   */
  void ScrollLeftToColumn(ULONG columId);

  /**
   * Scroll the text left by numChars chars and fills the gap at right
   * with the following chars
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when last char of longest line is displayed.
   */
  ULONG ScrollLeft(ULONG numChars);

  /**
   * Scroll the text right by numChars chars and fills the gap at left
   * with the previous chars
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when first char of text reached.
   */
  ULONG ScrollRight(ULONG numChars);

  /**
   * Scroll the text up by numLines lines and fills the gap at bottom
   * with the next lines.
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when end of text reached.
   */
  ULONG ScrollUp(ULONG numLines);

  /**
   * Scroll the text down by numLines lines and fills the gap at top
   * with the previous lines
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when start of text reached.
   */
  ULONG ScrollDown(ULONG numLines);

  /**
   * Print the diff file content in the text area at given position.
   */
  void PrintPageAt(ULONG left, ULONG top);

  /**
   * Print the diff file content in the text area.
   */
  void PrintPage();


  /**
   * Clear text area completely
   */
  void Clear();

  /**
   * Set the selected text
   */
  void AddSelectedText(ULONG fromLine, ULONG toLine, ULONG fromColumn, ULONG toColumn);


private:
  SelectableDiffFile m_DiffFile;
  DiffWindowRastports*& m_pRPorts;
  bool m_LineNumbersEnabled;
  ULONG m_MaxNumChars;
  TextSelection m_TextSelection;

  ULONG m_NumLines;           ///> Number of lines (as it's a diff view
                              ///> this should be equal for both files)

  UWORD m_FontWidth_pix;    ///> Width of the rastport text font
  UWORD m_FontHeight_pix;   ///> Height of the rastport text font
  UWORD m_FontBaseline_pix; ///> Baseline (from top) of the rastport text font


  ULONG m_AreaCharWidth;  ///> Max number of chars that fits into 
                          ///> the text area at current size

  ULONG m_AreaLineHeight; ///> Max number of lines that fits into 
                          ///> the text area at current size

  ULONG m_X;        ///> Index of leftmost char in displayed lines
                    ///> Is > 0 when text is horizontally scrolled

  ULONG m_Y;        ///> Index of topmost displayed line in document
                    ///> Is > 0 when text is vertically scrolled

  UWORD m_LineNumsWidth_chars;
  UWORD m_LineNumsWidth_pix;

  Rect m_HScrollRect;         ///> Area to be used for horizontal scroll
  Rect m_VScrollRect;         ///> Area to be used for vertical scroll

  /**
   * Print the given diff line at given y-position topEdge.
   */
  void printDiffLine(const DiffLine* pLeftLine,
                    WORD topEdge,
                    bool bHorizontallyScrolled = false,
                    int startIndex = -1,
                    int numChars = 0);

  /**
   * Calculate how many chars of given DiffLine must be print 
   * considering the current scroll position m_X, the current width of
   * the text area and the startIndex.
   */
  ULONG calcNumPrintChars(const DiffLine* pDiffLine, 
                          int count,
                          int startIndex);

  /**
   * Returns the appropriate rastport for a DiffLine with given
   * state to be printed in.
   */
  RastPort* diffStateToRastPort(DiffLine::LineState state);
};

#endif
