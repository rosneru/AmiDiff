#ifndef TEXT_WINDOW_H
#define TEXT_WINDOW_H

#include <exec/ports.h>
#include <intuition/screens.h>

#include "AppScreen.h"
#include "SimpleString.h"
#include "TextDocument.h"
#include "WindowBase.h"

/**
 * Class for a text window object. Can be created multiple times.
 *
 * @author Uwe Rosner
 * @date 23/09/2018
 */
class TextWindow : public WindowBase
{
public:
  TextWindow(AppScreen* p_pAppScreen);
  virtual ~TextWindow();

  /**
   * TODO make virtual?
   */
  void HandleIdcmp(ULONG p_Class, UWORD p_Code, APTR p_IAddress);


  /**
   * Reorganizes the window including re-calculating the scrollbars. If
   * needed also re-drawing the obscured text areas.
   *
   * This should be called from the application if the signal
   * IDCMP_NEWSIZE for this window is received.
   */
  void Resized();

  void Refresh();

  /**
   * Opens the window.
   *
   * @returns
   * false if opening fails
   */
  bool Open(struct MsgPort* p_pMsgPort);

  /**
   * Closes the window
   */
  virtual void Close();


  /**
   * Open a text file
   *
   * Extended method from base class DiffDocument
   *
   * @param p_FileName
   * Full file name with path for to be opened file. If empty a ASL
   * request will be opened asking the user for the file name.
   */
  virtual bool SetContent(TextDocument* p_pTextDocument);

  /**
   * This handles the Y-Changes triggered by the vertical scrollbar
   * of the window. It does so by completely re-drawing the text from
   * beginning with p_NewY on the top line.
   *
   * This method should be called from Application event loop if the
   * Y-proportional gadget attached to the window has been moved by
   * the user.
   */
  void YChangedHandler(size_t p_NewY);

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


protected:
  TextDocument* m_pDocument;


private:
  /**
   * IDs to help to interpret the events of this window's BOOPSI system
   * gadgets in the Application event loop.
   */
  enum GadgetId
  {
    GID_PropX,
    GID_PropY,
    GID_ArrowUp,
    GID_ArrowDown,
    GID_ArrowLeft,
    GID_ArrowRight,
  };

  enum LastScrollDirection
  {
    None,
    Upward,
    Downward,
  };

  SimpleString m_Title;

  size_t m_MaxWindowTextLines;  ///> Number of text lines that fit in window
  size_t m_Y;         ///> Index of currently first displayed text line

  WORD m_FontHeight;  ///> Height of current text font
  WORD m_ScrollXMin;  ///> Left x coordinate of scrolling area
  WORD m_ScrollYMin;  ///> Left y coordinate of scrolling area
  WORD m_ScrollXMax;  ///> Right x coordinate of scrolling area
  WORD m_ScrollYMax;  ///> Right y coordinate of scrolling area

  /**
   * Stores if the last scroll direction was upward or downward. Allows
   * the scroll methods scrollDownOneLine() and scrollUpOneLine() to
   * use GetPrevious() and GetNext() instead of GetIndexed() if the
   * same scroll direction is used again.
   *
   * Possibly makes scrolling by cursor keys faster at the bottom of
   * big files.
   */
  LastScrollDirection m_LastScrollDirection;

  struct Image* m_pLeftArrowImage;    ///> h-scrollbar left button image
  struct Image* m_pRightArrowImage;   ///> h-scrollbar right button image
  struct Image* m_pUpArrowImage;      ///> v-scrollbar up button image
  struct Image* m_pDownArrowImage;    ///> v-scrollbar down button image

  struct Gadget* m_pXPropGadget;      ///> horizontal scrollbar
  struct Gadget* m_pYPropGadget;      ///> vertical scrollbar
  struct Gadget* m_pLeftArrowButton;  ///> h-scrollbar left button
  struct Gadget* m_pRightArrowButton; ///> h-scrollbar right button
  struct Gadget* m_pUpArrowButton;    ///> v-scrollbar up button
  struct Gadget* m_pDownArrowButton;  ///> v-scrollbar down button

  struct TextAttr m_TextAttr;
  struct IntuiText m_IntuiText;

  /**
   * Calculates how many lines fit into current window size and sets
   * the member variable.
   */
  void calcMaxWindowTextLines();

  /**
   * Displays the given line at given y-position
   */
  void displayLine(SimpleString* p_pLine, WORD p_TopEdge);

  /**
   * Displays the complete file from current m_Y position as first line
   */
  void displayFile();

  bool scrollDownOneLine();
  bool scrollUpOneLine();
};


#endif
