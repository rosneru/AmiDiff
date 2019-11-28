#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "DiffDocument.h"
#include "DiffLine.h"
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
  DiffWindow(AppScreen& appScreen,
             struct MsgPort* pMsgPort,
             int& numOpenWindows);

  virtual ~DiffWindow();


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
                    InitialPosition initialPosition = WindowBase::IP_Center);


  /**
   * Open a text file
   *
   * Extended method from base class Document
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  bool SetContent(DiffDocument* pLeftDocument,
                  DiffDocument* pRightDocument);

  /**
   * Setting the data to be shown on the status bar.
   *
   * A reapienting of the status bar is triggered afterwards.
   *
   * @param diffTime
   * How long did comparing the files take (in milli seconds)
   *
   * @param numAdded
   * Number of added lines (sum of left  and right file)
   *
   * @param numChanged
   * Number of changed lines (sum of left  and right file)
   *
   * @param numDeleted
   * Number of deleted lines (sum of left  and right file)
   *
   */
  void SetStatusBar(long diffTime,
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
  virtual bool HandleIdcmp(ULONG msgClass,
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

private:
  DiffDocument* m_pLeftDocument;
  DiffDocument* m_pRightDocument;

  struct Gadget* m_pGadgetsHeader;
  struct Gadget* m_pGadTxtLeftFile;
  struct Gadget* m_pGadTxtRightFile;

  unsigned short m_TextFontWidth_pix;  ///> Width of the rastport text font
  unsigned short m_TextFontHeight_pix; ///> Height of the rastport text font

  unsigned short m_X;   ///> Index of leftmost char in displayed lines.
                        ///> Is > 0 when text is horizontally scrolled.

  unsigned long m_Y;    ///> Index of topmost displayed line in document.
                        ///> Is > 0 when text is vertically scrolled.

  unsigned short m_MaxTextAreaChars;  ///> Max fitting chars in each text area.
                                      ///> Depending on font and text area size.

  unsigned long m_MaxTextAreaLines; ///> Max fitting lines in each text area.
                                    ///> Depending on font and window size.

  unsigned short m_MaxLineLength;   ///> Longest line length (in chars) either
                                    ///> in left or in right file

  unsigned short m_IndentX;  ///> X-distance of the text areas to window borders
  unsigned short m_IndentY;  ///> Y-distance of the text areas to window borders

  unsigned short m_TextArea1Left;   ///> X-position of the 1st text area
  unsigned short m_TextArea2Left;   ///> X-position of the 2nd text area
  unsigned short m_TextAreasTop;    ///> Y-position of the text areas
  unsigned short m_TextAreasWidth;  ///> Width of each text area
  unsigned short m_TextAreasHeight; ///> Height of the text areas (equally heigh)

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

  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  void calcSizes();

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
  void paintDocument(bool fromStart = true);

  /**
   * Prints the given line (p_pLeftLine and p_pRightLine) at given
   * y-position p_TopEdge.
   */
  void paintLine(const DiffLine* pLeftLine,
                 const DiffLine* pRightLine,
                 WORD topEdge,
                 int startIndex = -1,
                 int numChars = 0);

  /**
   * Paints the basic window content: the 2 BevelBoxes for the original
   * (left) and changed (right) diff file, the Document names and the
   * state bar at the bottom.
   */
  void paintWindowDecoration();

  /**
   * Displays the names of the documents above the two text areas
   */
  void paintDocumentNames();

  /**
   * Displays a status bar with statistical information and a legend
   * below the two text areas
   */
  void paintStatusBar();

  /**
   * Returns the appropriate pen for a given ColorName
   */
  LONG colorNameToPen(DiffDocument::ColorName colorName);

  /**
   * Scrolls the text in both text areas left by numChars chars and
   * fills the gap at right with the following chars
   *
   * NOTE: Does *not* change the current left line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when last char of longest line is displayed.
   */
  size_t scrollLeft(int numChars);

  /**
   * Scrolls the text in both text areas right by numChars chars and
   * fills the gap at left with the previous chars
   *
   * NOTE: Does *not* change the current top line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when first char of text reached.
   */
  size_t scrollRight(int numChars);

  /**
   * Scrolls the text in both text areas down by numLines lines and
   * fills the gap at top with the previous lines
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when start of text reached.
   */
  size_t scrollDown(int numLines);

  /**
   * Scrolls the text in both text areas up by numLines lines and fills
   * the gap at bottom with the next lines.
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when end of text reached.
   */
  size_t scrollUp(int numLines);
};


#endif
