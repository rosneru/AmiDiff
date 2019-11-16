#ifndef DIFF_WINDOW_H
#define DIFF_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
#include "DiffDocument.h"
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
  DiffWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort,
             int& p_NumWindowsOpen);

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
  virtual bool Open(const APTR p_pMenuItemDisableAtOpen = NULL);


  /**
   * Open a text file
   *
   * Extended method from base class Document
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  bool SetContent(DiffDocument* p_pLeftDocument,
    DiffDocument* p_pRightDocument);

  /**
   * Setting the data to be shown on the status bar.
   * 
   * A reapienting of the status bar is triggered afterwards.
   * 
   * @param p_DiffTime
   * How long did comparing the files take (in milli seconds)
   * 
   * @param p_NumAdded
   * Number of added lines (sum of left  and right file)
   * 
   * @param p_NumChanged
   * Number of changed lines (sum of left  and right file)
   * 
   * @param p_NumDeleted
   * Number of deleted lines (sum of left  and right file)
   * 
   */
  void SetStatusBar(long p_DiffTime, int p_NumAdded, int p_NumChanged, 
                    int p_NumDeleted);

  /**
   * Set the number of changes to be displayed at the status bar.
   * A reapienting of the status bar is triggered afterwards.
   * 
   */
  void SetNumChanges(int p_NumAdded,int p_NumChanged, int p_NumDeleted);



  /**
   * Handles given IDCMP event.
   *
   * @returns If this event was handled: true; else: false.
   */
  virtual bool HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

  /**
   * This handles the x-changes triggered by the horizontal scrollbar
   * of the window. It is called from the handleIDCMP() method of
   * ScrollbarWindow.
   */
  void XChangedHandler(size_t p_NewX);

  /**
   * This handles the y-changes triggered by the vertical scrollbar
   * of the window. It is called from the handleIDCMP() method of
   * ScrollbarWindow.
   */
  void YChangedHandler(size_t p_NewY);

  /**
   * Increases the X position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param p_IncreaseBy
   * Amount to increase the x-position by
   *
   * @param p_bTriggeredByScrollbarPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void XIncrease(size_t p_IncreaseBy, bool p_bTriggeredByScrollbarPot);

  /**
   * Decreases the X position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param p_DecreaseBy
   * Amount to decrease the x-position by
   *
   * @param p_bTriggeredByScrollbarPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void XDecrease(size_t p_DecreaseBy, bool p_bTriggeredByScrollbarPot);

  /**
   * Increases the Y position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param p_IncreaseBy
   * Amount to increase the y-position by
   *
   * @param p_bTriggeredByScrollbarPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void YIncrease(size_t p_IncreaseBy, bool p_bTriggeredByScrollbarPot);

  /**
   * Decreases the Y position of the text by the given amount and
   * performs a scrolling as needed.
   *
   * @param p_DecreaseBy
   * Amount to decrease the y-position by
   *
   * @param p_bTriggeredByScrollbarPot
   * If the call is triggered by moving the scrollbar pot: true
   * If the call is triggered by other sources: false
   */
  void YDecrease(size_t p_DecreaseBy, bool p_bTriggeredByScrollbarPot);

private:
  DiffDocument* m_pLeftDocument;
  DiffDocument* m_pRightDocument;

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

  /**
   * Initializes some window specific features. Gadgets, etc.
   */
  void initialize();

  /**
   * Calculates some inner window sizes which is needed after window
   * opening and after resizing.
   */
  void calcSizes();

  /**
   * Displays the left and right diff file inside the main text area.
   * The file is displayed starting from current text position m_Y as
   * first line at the very top of the text area.
   *
   * @param  p_bDisplayFromStart
   * When true: Before printing the documents the line and column
   *   indices m_X and m_Y are reset to 0. So the document will be
   *   displayed from start.
   * When false: Printing starts at current position m_Y.
   */
  void paintDocument(bool  p_bDisplayFromStart = true);

  /**
   * Prints the given line (p_pLeftLine and p_pRightLine) at given
   * y-position p_TopEdge.
   */
  void paintLine(const SimpleString* p_pLeftLine,
    const SimpleString* p_pRightLine, WORD p_TopEdge,
    int p_StartCharIndex = -1, int p_NumChars = 0);

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
   *  below the two text areas
   */
  void paintStatusBar();

  /**
   * Gets an appropriate pen from a given ColorName
   */
  LONG colorNameToPen(DiffDocument::ColorName p_pColorName);

  /**
   * Scrolls the current text in the text area left by
   * p_ScrollNumCharsLeft chars and fills the gap at right with the
   * following chars
   *
   * NOTE: Does *not* change the current left line position m_X!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when last char of longest line is displayed.
   */
  size_t scrollNCharsLeft(int p_ScrollNumCharsLeft);

  /**
   * Scrolls the current text in the text area right by
   * p_ScrollNumCharsRight chars and fills the gap at left with the
   * previous chars
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when first char of text reached.
   */
  size_t scrollNCharsRight(int p_ScrollNumCharsRight);

  /**
   * Scrolls the current text in the text area down by
   * p_ScrollNumLinesDown lines and fills the gap at top with the
   * previous lines
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when start of text reached.
   */
  size_t scrollNLinesDown(int p_ScrollNumLinesDown);

  /**
   * Scrolls the current text in the text area up by
   * p_ScrollUpNumLinesUp lines and fills the gap at bottom with the
   * next lines
   *
   * NOTE: Does *not* change the current top line position m_Y!
   *
   * @returns Number of lines scrolled. Can be smaller than expected
   * when end of text reached.
   */
  size_t scrollNLinesUp(int p_ScrollUpNumLinesUp);
};


#endif