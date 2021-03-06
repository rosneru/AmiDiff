#ifndef TEXT_WINDOW_H
#define TEXT_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include <string>
#include "TextDocument.h"
#include "ScrollbarWindow.h"

/**
 * Class for a text window object. Can be created multiple times.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class TextWindow : public ScrollbarWindow
{
public:
  TextWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort,
             int& p_NumWindowsOpen);
  virtual ~TextWindow();

  /**
   * Reorganizes the window including re-calculating the scrollbars.
   * If needed also re-drawing the obscured text areas.
   *
   * This should be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  virtual void performResize();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  virtual bool Open(const APTR p_pMenuItemDisableAtOpen = NULL);

  /**
   * Handles given IDCMP event.
   *
   * @returns If this event was handled: true; else: false.
   */
  virtual bool handleIDCMP(ULONG p_Class, UWORD p_Code, APTR p_IAddress);


  /**
   * Open a text file
   *
   * Extended method from base class DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  bool setDocument(TextDocument* p_pTextDocument);

  /**
   * This handles the x-changes triggered by the horizontal scrollbar
   * of the window. It is called from the handleIDCMP() method of
   * ScrollbarWindow.
   */
  void XChangedHandler(size_t p_NewY);

  /**
   * This handles the y-changes triggered by the vertical scrollbar
   * of the window. It is called from the handleIDCMP() method of
   * ScrollbarWindow.
   */
  virtual void handleYChange(size_t p_NewY);

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
  void handleXIncrease(size_t p_IncreaseBy, bool p_bTriggeredByScrollbarPot);

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
  void handleXDecrease(size_t p_DecreaseBy, bool p_bTriggeredByScrollbarPot);

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
  void handleYIncrease(size_t p_IncreaseBy, bool p_bTriggeredByScrollbarPot);

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
  void handleYDecrease(size_t p_DecreaseBy, bool p_bTriggeredByScrollbarPot);


private:
  TextDocument* m_pDocument;

  struct TextAttr m_TextAttr;

  int m_TextFontWidth_pix;  ///> Width of the rastport text font
  int m_TextFontHeight_pix; ///> Height of the rastport text font

  int m_X;  ///> Index of leftmost char in displayed lines.
            ///> Is > 0 when text is horizontally scrolled.

  int m_Y;  ///> Index of topmost displayed line in document.
            ///> Is > 0 when text is vertically scrolled.

  int m_MaxWinChars;      ///> Max fitting lines in each text area.
                          ///> Depending on font and window size.

  int m_MaxWinLines;      ///> Max fitting lines in each text area.
                          ///> Depending on font and window size.

  int m_IndentX;  ///> X-distance of the text area to window borders
  int m_IndentY;  ///> Y-distance of the text area to window borders

  int m_TextAreaLeft;     ///> X-position of the 1st text area
  int m_TextAreaTop;      ///> Y-position of the text area
  int m_TextAreaWidth;    ///> Width of each text area
  int m_TextAreaHeight;   ///> Height of the text area

  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  virtual void initialize();

  /**
   * Calculates how many lines fit into current window size and sets
   * the member variable.
   */
  void calcSizes();

  /**
   * Displays the complete file from current m_Y position as first line
   *
   * @param p_bStartFromTop
   * When true: Prints the document starting with line index 0.
   * When false: Printing starts at current position m_Y.
   */
  void paintDocument(bool p_bStartFromTop = true);

  /**
   * Prints the given line at given y-position p_TopEdge.
   */
    void paintLine(const std::string* p_pLine, WORD p_TopEdge);

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
