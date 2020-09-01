#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include <string>

#include <exec/ports.h>
#include <intuition/screens.h>

#include "ADiffViewPens.h"
#include "DiffDocument.h"
#include "DiffLine.h"
#include "DiffWindowRastports.h"
#include "DiffWindowTextArea.h"
#include "OpenScreenBase.h"
#include "ScrollbarWindow.h"

/**
 * Class for a diff window object. Can be created multiple times.
 *
 * @author Uwe Rosner
 * @date 16/11/2018
 */
class DiffWindow : public ScrollbarWindow
{
public:
  DiffWindow(ScreenBase& screen,
             const ADiffViewPens& pens,
             struct MsgPort* pIdcmpMsgPort,
             MenuBase* pMenu);

  virtual ~DiffWindow();

  /**
   * Reorganizes the window including re-calculating the scrollbars. If
   * needed also re-drawing the obscured text areas.
   *
   * This should be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  void Resized();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  virtual bool Open(InitialPosition initialPos = WindowBase::IP_Center);


  /**
   * Set the diff document to be displayed in DiffWindow.
   */
  bool SetContent(DiffDocument* pDiffDocument);


  /**
   * Navigate to the next difference.
   */
  void NavigateToNextDiff();

  /**
   * Navigate to the previous difference.
   */
  void NavigateToPrevDiff();

  /**
   * Extends method of base class ScrollbarWindow.
   */
  virtual void HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress);


  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void XChangedHandler(size_t newX);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void YChangedHandler(size_t newY);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void XIncrease(size_t numChars, bool bTriggeredByScrollPot);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void XDecrease(size_t numChars, bool bTriggeredByScrollPot);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void YIncrease(size_t numLines, bool bTriggeredByScrollPot);

  /**
   * Implemented abstract method of base class ScrollbarWindow.
   */
  void YDecrease(size_t numLines, bool bTriggeredByScrollPot);


private:
  const ADiffViewPens& m_Pens;

  DiffWindowRastports* m_pRPorts;
  DiffDocument* m_pDocument;
  char m_EmptyChar;

  struct Gadget* m_pLastParentGadget;
  struct Gadget* m_pGadtoolsContext;
  struct Gadget* m_pGadTxtLeftFile;
  struct Gadget* m_pGadTxtRightFile;

  long m_NumDifferences;      ///> Number of differences

  UWORD m_LineNumsWidth_chars;
  UWORD m_LineNumsWidth_pix;

  UWORD m_FontWidth_pix;    ///> Width of the rastport text font
  UWORD m_FontHeight_pix;   ///> Height of the rastport text font
  UWORD m_FontBaseline_pix; ///> Baseline (from top) of the rastport text font

  ULONG m_X;        ///> Index of leftmost char in displayed lines
                    ///> Is > 0 when text is horizontally scrolled

  ULONG m_Y;        ///> Index of topmost displayed line in document
                    ///> Is > 0 when text is vertically scrolled

  ULONG m_MaxTextAreaChars;   ///> Max fitting chars in each text area
                              ///> Depending on font and text area size

  ULONG m_MaxTextAreaLines;   ///> Max fitting lines in each text area
                              ///> Depending on font and window size

  ULONG m_NumLines;           ///> Number of lines (as it's a diff view
                              ///> this should be equal for both files)


  ULONG m_IndentX;  ///> X-distance of the text areas to window borders
  ULONG m_IndentY;  ///> Y-distance of the text areas to window borders

  DiffWindowTextArea m_TextArea1;
  DiffWindowTextArea m_TextArea2;

  char m_ChangedText[20];
  char m_AddedText[20];
  char m_DeletedText[20];
  char m_StatusBarText[60];

  struct TextAttr m_TextAttr;

  //
  // The next two are called from HandleIDCMP() to get that method not
  // overblown.
  //
  void handleGadgetEvent(struct Gadget* pGadget);
  void handleVanillaKey(UWORD code);


  void createGadgets();

  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  void calcSizes();

  /**
   * Resizes the gadgets. Should be called after the window size has
   * changed. As gadget-resizing or re-positioning is not supported with
   * Gadtools, the gadgets are re-created completely.
   */
  void resizeGadgets();

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

  /**
   * Paints the basic window content: the 2 BevelBoxes for the original
   * (left) and changed (right) diff file, the Document names and the
   * state bar at the bottom.
   */
  void paintWindowDecoration();


  /**
   * Displays a status bar with statistical information and a legend
   * below the two text areas
   */
  void paintStatusBar();

  /**
   * Calculates how many chars of given DiffLine must be print 
   * considering the current scroll position m_X, the current width of
   * the text area and the startIndex.
   */
  LONG calcNumPrintChars(const DiffLine* pDiffLine, 
                         int count,
                         int startIndex);

  /**
   * Returns the appropriate rastport for a DiffLine with given
   * state to be printed in.
   */
  RastPort* diffStateToRastPort(DiffLine::LineState state);

  /**
   * Scrolls the text in both text areas left by numChars chars and
   * fills the gap at right with the following chars
   *
   * NOTE: Does *not* change the current left line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when last char of longest line is displayed.
   */
  size_t scrollLeft(size_t numChars);

  /**
   * Scrolls the text in both text areas right by numChars chars and
   * fills the gap at left with the previous chars
   *
   * NOTE: Does *not* change the current top line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when first char of text reached.
   */
  size_t scrollRight(size_t numChars);

  /**
   * Scrolls the text in both text areas down by numLines lines and
   * fills the gap at top with the previous lines
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when start of text reached.
   */
  size_t scrollDown(size_t numLines);

  /**
   * Scrolls the text in both text areas up by numLines lines and fills
   * the gap at bottom with the next lines.
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when end of text reached.
   */
  size_t scrollUp(size_t numLines);
};


#endif
