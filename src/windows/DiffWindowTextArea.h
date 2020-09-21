#ifndef DIFF_WINDOW_TEXT_AREA_H
#define DIFF_WINDOW_TEXT_AREA_H

#include <exec/types.h>
#include <intuition/intuition.h>

#include "DiffLine.h"
#include "DiffOutputFileBase.h"

#include "DiffWindowRastports.h"
#include "Rect.h"

class DiffWindowTextArea : public Rect
{
public:
  DiffWindowTextArea(DiffWindowRastports*& pRPorts, 
                     const UWORD& lineNumbersWidth_pix);

  virtual ~DiffWindowTextArea();

  /**
   * Set the document and paint initially.
   * 
   * FIXME: Move this to the constructor
   */
  void SetDocument(const DiffOutputFileBase* pDiffFile);

  void ScrollToTopRow(size_t rowId);
  void ScrollToLeftColumn(size_t columId);

  /**
   * Sets width and hight and also calculates the HScroll and VScroll
   * rects wgich depent on the text width of the max possible text width
   * of this text area.
   */
  void SetWidthHeightScroll(long width, long height, long maxTextWidth_pix);

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

  ULONG m_NumLines;           ///> Number of lines (as it's a diff view
                              ///> this should be equal for both files)

  ULONG m_MaxTextAreaChars;   ///> Max fitting chars in each text area
                              ///> Depending on font and text area size

  ULONG m_MaxTextAreaLines;   ///> Max fitting lines in each text area
                              ///> Depending on font and window size

  ULONG m_X;        ///> Index of leftmost char in displayed lines
                    ///> Is > 0 when text is horizontally scrolled

  ULONG m_Y;        ///> Index of topmost displayed line in document
                    ///> Is > 0 when text is vertically scrolled

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
   * Displays the left and right diff file inside the main text area.
   * The file is displayed starting from current text position m_Y as
   * first line at the very top of the text area.
   *
   * @param fromStart When true: Before printing the documents the line
   * and column indices m_X and m_Y are reset to 0. So the document will
   * be displayed from start. When false: Printing starts at current
   * position m_Y.
   */
  void paintDocuments(bool bFromStart = true);

  /**
   * Prints the given line (pLeftLine and pRightLine) at given
   * y-position topEdge.
   */
  void paintLine(const DiffLine* pLeftLine,
                 const DiffLine* pRightLine,
                 WORD topEdge,
                 bool bHorizontallyScrolled = false,
                 int startIndex = -1,
                 int count = 0);

};

#endif
