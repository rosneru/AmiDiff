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
  void SetSize(ULONG width, ULONG height);

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
  void AddSelectedText(ULONG lineId, ULONG fromColumn, ULONG toColumn);


private:
  SelectableDiffFile m_DiffFile;
  DiffWindowRastports*& m_pRPorts;
  bool m_LineNumbersEnabled;
  ULONG m_LongestLineChars; ///> Number of chars of the longest line of DiffFile.

  UWORD m_FontWidth_pix;    ///> Width of the rastport text font
  UWORD m_FontHeight_pix;   ///> Height of the rastport text font
  UWORD m_FontBaseline_pix; ///> Baseline (from top) of the rastport text font


  ULONG m_AreaMaxChars; ///> Max number of chars that fits in at current size
  ULONG m_AreaMaxLines; ///> Max number of lines that fits in at current size

  ULONG m_X;  ///> Position of left text column in display (> 0 when x scroll)
  ULONG m_Y;  ///> Position of top line in display (> 0 when y scroll)

  UWORD m_LineNumsWidth_chars;
  UWORD m_LineNumsWidth_pix;

  Rect m_HScrollRect;     ///> Horizontal scroll region
  Rect m_VScrollRect;     ///> Vertical scroll region

  /**
   * Print the given diff line at given y-position topEdge.
   */
  void printDiffLine(ULONG lineId, long lineTop, long onlyNumChars = 0);

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
