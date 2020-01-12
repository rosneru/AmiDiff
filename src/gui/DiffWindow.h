#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AScreen.h"
#include "DiffDocument.h"
#include "DiffLine.h"
#include "LinkedList.h"
#include "SimpleString.h"
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
  DiffWindow(AScreen& appScreen,
             struct MsgPort*& pIdcmpMsgPort,
             int& numOpenWindows);

  virtual ~DiffWindow();


  /**
   * When called before opening, the line numbers of the files are
   * displayed depending on the given value.
   */
  void SetLineNumbersVisible(bool bEnabled);


  /**
   * Reorganizes the window including re-calculating the scrollbars.
   * If needed also re-drawing the obscured text areas.
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
  virtual bool Open(const APTR pMenuItemDisableAtOpen = NULL,
                    InitialPosition initialPos = WindowBase::IP_Center);


  /**
   * Set the main content for the DiffWindow.
   *
   * @param pLeftDocument
   * The DiffDocument to be displayed on the left side. Should be
   * created with the resulting left DiffFile after DiffEngine.Diff()
   * has run successfully.
   *
   * @param pRightDocument
   * The DiffDocument to be displayed on the right side. Should be
   * created with the resulting right DiffFile after DiffEngine.Diff()
   * has run successfully.
   *
   * @param pDiffStartIdxs
   * An array, containing the y-start-idx of each diff
   *
   * @param numDifferences
   * The number of differences. Summarized the changed, added and
   * inserted lines. This is also the number of items of the array
   * pDiffStartIdxs.
   */
  bool SetContent(DiffDocument* pDiffDocument,
                  LinkedList* pDiffStartIdxsList,
                  long diffTime,
                  int numAdded,
                  int numChanged,
                  int numDeleted);


  /**
   * Set the number of changes to be displayed at the status bar.
   * A reapienting of the status bar is triggered afterwards.
   *
   */
  void SetNumChanges(int numAdded,
                     int numChanged,
                     int numDeleted);



  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  virtual void HandleIdcmp(ULONG msgClass,
                           UWORD msgCode,
                           APTR pItemAddress);

  /**
   * This handles the x-changes triggered by the horizontal scrollbar
   * of the window. It is called from the handleIDCMP() method of
   * ScrollbarWindow.
   */
  void XChangedHandler(size_t newX);

  /**
   * This handles the y-changes triggered by the vertical scrollbar
   * of the window. It is called from the handleIDCMP() method of
   * ScrollbarWindow.
   */
  void YChangedHandler(size_t newY);

  /**
   * Increases the X position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param numChars
   * Amount to increase the x-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void XIncrease(size_t numChars, bool bTriggeredByScrollPot);

  /**
   * Decreases the X position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param numChars
   * Amount to decrease the x-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void XDecrease(size_t numChars, bool bTriggeredByScrollPot);

  /**
   * Increases the Y position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param numLines
   * Amount to increase the y-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void YIncrease(size_t numLines, bool bTriggeredByScrollPot);

  /**
   * Decreases the Y position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param numLines
   * Amount to decrease the y-position by
   *
   * @param bTriggeredByScrollPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void YDecrease(size_t numLines, bool bTriggeredByScrollPot);

  void NavigateToNextDiff();
  void NavigateToPrevDiff();

private:
  DiffDocument* m_pDocument;
  char m_EmptyChar;

  struct RastPort m_RPortAPenBackgr;
  struct RastPort m_RPortLineNum;
  struct RastPort m_RPortTextDefault;
  struct RastPort m_RPortTextRedBG;
  struct RastPort m_RPortTextGreenBG;
  struct RastPort m_RPortTextYellowBG;

  struct Gadget* m_pLastParentGadget;
  struct Gadget* m_pGadtoolsContext;
  struct Gadget* m_pGadTxtLeftFile;
  struct Gadget* m_pGadTxtRightFile;

  LinkedList* m_pDiffStartIdxsList; ///> A list of size_t*, which content the y-start-idx of each diff
  bool m_bNoNavigationDone;
  long m_NumDifferences;      ///> Number of differences

  bool m_bShowLineNumbers;
  UWORD m_LineNumsWidth_chars;
  UWORD m_LineNumsWidth_pix;
  SimpleString m_LineNumberEmpty;

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

  ULONG m_TextArea1Left;      ///> X-position of the 1st text area
  ULONG m_TextArea2Left;      ///> X-position of the 2nd text area
  ULONG m_TextAreasTop;       ///> Y-position of the text areas
  ULONG m_TextAreasWidth;     ///> Width of each text area
  ULONG m_TextAreasHeight;    ///> Height of each of the text areas

  LONG m_ScrollArea1XMinHScroll;
  LONG m_ScrollArea1XMinVScroll;
  LONG m_ScrollArea1XMax;
  LONG m_ScrollArea2XMinHScroll;
  LONG m_ScrollArea2XMinVScroll;
  LONG m_ScrollArea2XMax;
  LONG m_ScrollAreasYMin;
  LONG m_ScrollAreasYMax;

  SimpleString m_ChangedText;
  SimpleString m_AddedText;
  SimpleString m_DeletedText;
  SimpleString m_StatusBarText;

  struct TextAttr m_TextAttr;

  //
  // The next two are called from HandleIDCMP() to get that method not
  // overblown.
  //
  void handleGadgetEvent(struct Gadget* pGadget);
  void handleVanillaKey(UWORD code);


  /**
   * Initializes some window specific features. Gadgets, etc.
   */
  void initialize();

  void createGadgets();

  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  void calcSizes();

  /**
   * Resizes the gadgets. Should be called after the window size has
   * changed. As gadget-resizing or re-positioning is not supported
   * with Gadtools, the gadgets are re-created completely.
   */
  void resizeGadgets();

  /**
   * Displays the left and right diff file inside the main text area.
   * The file is displayed starting from current text position m_Y as
   * first line at the very top of the text area.
   *
   * @param fromStart
   * When true: Before printing the documents the line and column
   *   indices m_X and m_Y are reset to 0. So the document will be
   *   displayed from start.
   * When false: Printing starts at current position m_Y.
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
                 int numChars = 0);

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
   * Returns the appropriate pen for a given ColorName
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
