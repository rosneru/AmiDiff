#ifndef DIFF_WINDOW_TEXT_AREA_H
#define DIFF_WINDOW_TEXT_AREA_H

#include <exec/types.h>
#include <graphics/text.h>
#include <intuition/intuition.h>

#include "DiffLine.h"
#include "DiffOutputFileBase.h"

#include "DiffWindowRastports.h"
#include "Rect.h"

class DiffWindowTextArea : public Rect
{
public:
  DiffWindowTextArea(DiffWindowRastports*& pRPorts);

  virtual ~DiffWindowTextArea();

  ULONG X() const;
  ULONG Y() const;

  ULONG NumVisibleChars() const;
  ULONG NumVisibleLines() const;

  /**
   * Set the document and paint initially.
   * 
   * FIXME: Move this to the constructor
   */
  void SetDocument(const DiffOutputFileBase* pDiffFile, 
                   struct TextFont* pTextFont,
                   ULONG maxNumChars,
                   bool lineNumbersEnabled);

  /**
   * Set the dimensions of this text area.
   */
  void SetSizes(ULONG width, 
                ULONG height);


  void ScrollTopToRow(size_t rowId);
  void ScrollLeftToColumn(size_t columId);

  /**
   * Scrolls the text left by numChars chars and fills the gap at right
   * with the following chars
   *
   * NOTE: Does *not* change the current left line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when last char of longest line is displayed.
   */
  size_t scrollLeft(size_t numChars);

  /**
   * Scrolls the text right by numChars chars and fills the gap at left
   * with the previous chars
   *
   * NOTE: Does *not* change the current top line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when first char of text reached.
   */
  size_t scrollRight(size_t numChars);

  /**
   * Scrolls the text down by numLines lines and fills the gap at top
   * with the previous lines
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when start of text reached.
   */
  size_t scrollDown(size_t numLines);

  /**
   * Scrolls the text up by numLines lines and fills the gap at bottom
   * with the next lines.
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when end of text reached.
   */
  size_t scrollUp(size_t numLines);

  /**
   * Displays the diff file inside the main text area. The file is
   * displayed starting from current text position m_Y as first line at
   * the very top of the text area.
   *
   * @param fromStart When true: Before printing the documents the line
   * and column indices m_X and m_Y are reset to 0. So the document will
   * be displayed from start. When false: Printing starts at current
   * position m_Y.
   */
  void paintDocuments(bool bFromStart = true);


  /**
   * Clear text area completely
   */
  void Clear();

  /**
   * Returns the rect of the scrolling area for horizontal scrolling.
   */
  const Rect& HScroll();

  /**
   * Returns the rect of the scrolling area for vertical scrolling.
   */
  const Rect& VScroll();

private:
  DiffWindowRastports*& m_pRPorts;
  const DiffOutputFileBase* m_pDiffFile;
  const UWORD& m_LineNumbersWidth_pix;
  Rect m_HScrollRect;
  Rect m_VScrollRect;

  bool m_LineNumbersEnabled;

  ULONG m_NumLines;           ///> Number of lines (as it's a diff view
                              ///> this should be equal for both files)

  ULONG m_MaxNumChars;

  ULONG m_MaxTextAreaChars;   ///> Max fitting chars in each text area
                              ///> Depending on font and text area size

  ULONG m_MaxTextAreaLines;   ///> Max fitting lines in each text area
                              ///> Depending on font and window size

  ULONG m_X;        ///> Index of leftmost char in displayed lines
                    ///> Is > 0 when text is horizontally scrolled

  ULONG m_Y;        ///> Index of topmost displayed line in document
                    ///> Is > 0 when text is vertically scrolled

  UWORD m_LineNumsWidth_chars;
  UWORD m_LineNumsWidth_pix;

  UWORD m_FontWidth_pix;    ///> Width of the rastport text font
  UWORD m_FontHeight_pix;   ///> Height of the rastport text font
  UWORD m_FontBaseline_pix; ///> Baseline (from top) of the rastport text font

  /**
   * Prints the given line at given y-position topEdge.
   */
  void paintLine(const DiffLine* pLeftLine,
                 WORD topEdge,
                 bool bHorizontallyScrolled = false,
                 int startIndex = -1,
                 int count = 0);

  /**
   * Calculates how many chars of given DiffLine must be print 
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
