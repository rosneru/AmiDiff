#ifndef TEXT_WINDOW_H
#define TEXT_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
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
  TextWindow(AppScreen& p_AppScreen, struct MsgPort* p_pMsgPort);
  virtual ~TextWindow();

  /**
   * Reorganizes the window including re-calculating the scrollbars.
   * If needed also re-drawing the obscured text areas.
   *
   * This should be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  virtual void Resized();

  virtual void Refresh();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  virtual bool Open(APTR p_pMenuItemDisableAtOpen = NULL);

  /**
   * Open a text file
   *
   * Extended method from base class DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  bool SetContent(TextDocument* p_pTextDocument);

  /**
   * This handles the Y-Changes triggered by the vertical scrollbar
   * of the window. It does so by completely re-drawing the text from
   * beginning with p_NewY on the top line.
   *
   * This method should be called from Application event loop if the
   * Y-proportional gadget attached to the window has been moved by
   * the user.
   */
  virtual void YChangedHandler(size_t p_NewY);

  /**
   * Increases the Y position of the text by 1 and performs a scrolling
   * by one line.  Should be called from the Application event loop
   * when the cursor down key was received.
   */
  void YIncrease();

  /**
   * Decreases the Y position of the text by 1 and performs a scrolling
   * by one line.  Should be called from the Application event loop
   * when the cursor up key was received.
   */
  void YDecrease();


private:
  /**
   * Initializes some window specific feature. Gadgets, etc.
   */
  virtual void initialize();

  /**
   * Handles given IDCMP event.
   * 
   * @returns If this event was handled: true; else: false.
   */
  virtual bool handleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);

  /**
   * Calculates how many lines fit into current window size and sets
   * the member variable.
   */
  void calcSizes();

  /**
   * Displays the complete file from current m_Y position as first line
   */
  virtual void displayFile();

  bool scrollDownOneLine();
  bool scrollUpOneLine();

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

private:
  TextDocument* m_pDocument;
  
  struct TextAttr m_TextAttr;
  struct IntuiText m_IntuiText;

  int m_MaxTextLines;   ///> Number of text lines that fit in window
  int m_Y;              ///> Index of currently first displayed text line

  int m_TextAreaLeft;   ///> Left x coordinate of text area
  int m_TextAreaTop;    ///> Top y coordinate of text area
  int m_TextAreaRight;  ///> Right x coordinate of text area
  int m_TextAreaBottom; ///> Bottom y coordinate of text area

  /**
   * Displays the given line at given y-position
   */
  void displayLine(const SimpleString* p_pLine, WORD p_TopEdge);
};


#endif
